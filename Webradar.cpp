#include <unordered_set>
#include <iostream>
#include <mutex>

#include "Webradar.h"
#include "include.h"

// WebRadar 构造函数：初始化 Mongoose 管理器
WebRadar::WebRadar() {
    mg_mgr_init(&mgr);
}

void WebRadar::SendFile(struct mg_connection* c, struct mg_http_message* hm) {
    // 处理以 /maps/ 开头的静态资源请求
    const char* mapsPrefix = "/maps/";
    size_t prefixLen = strlen(mapsPrefix);

    if (strncmp(hm->uri.buf, mapsPrefix, prefixLen) == 0) {
        char filePath[512] = { 0 };
        const char* filename = hm->uri.buf + prefixLen;  // 去掉 /maps/ 前缀

        // 读取文件名直到空白字符，防止带入多余符号
        size_t filenameLen = 0;
        while (filename[filenameLen] && filename[filenameLen] != ' ' && filename[filenameLen] != '\r' && filename[filenameLen] != '\n') {
            filenameLen++;
        }

        // 拼接生成本地文件路径
        snprintf(filePath, sizeof(filePath), "./maps/%.*s", (int)filenameLen, filename);

        // 检查文件是否存在
        FILE* f = fopen(filePath, "rb");
        if (!f) {
            printf("[WebRadar] File not found: %s\n", filePath);
            mg_http_reply(c, 404, "", "File not found");
            return;
        }
        fclose(f);

        struct mg_http_serve_opts opts = { 0 };
        opts.mime_types = "png=image/png";
        opts.root_dir = ".";

        mg_http_serve_file(c, hm, filePath, &opts);
    }
}

// 解析查询字符串中的指定参数
bool extract_query_param(const struct mg_str* query, const char* param_name, char* value, size_t value_size) {
    char param_with_equal[32] = { 0 };
    snprintf(param_with_equal, sizeof(param_with_equal), "%s=", param_name);
    size_t param_len = strlen(param_with_equal);

    for (size_t i = 0; i + param_len < query->len; i++) {
        if (i > 0 && query->buf[i - 1] != '&' && query->buf[i - 1] != '?') continue;

        if (strncmp(query->buf + i, param_with_equal, param_len) == 0) {
            // 记录参数值
            size_t j = i + param_len;
            size_t value_pos = 0;

            // 遇到 & 或缓冲区上限就停止复制
            while (j < query->len && query->buf[j] != '&' && value_pos < value_size - 1) {
                value[value_pos++] = query->buf[j++];
            }
            value[value_pos] = '\0';
            return true;
        }
    }

    return false;
}

// WebRadar 的 HTTP 事件处理
void WebRadar::EventHandler(struct mg_connection* c, int ev, void* ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*)ev_data;
        WebRadar* radar = (WebRadar*)c->fn_data;

        if (mg_strcmp(hm->uri, mg_str("/")) == 0) {
            radar->SendHtml(c);
        }
        else if (mg_strcmp(hm->uri, mg_str("/data")) == 0) {
            radar->SendData(c);
        }
        // 处理语言切换请求
        else if (strncmp(hm->uri.buf, "/setlang", 8) == 0) {
            // 解析 lang 参数
            char lang_value[2] = { 0 };
            if (extract_query_param(&hm->query, "lang", lang_value, sizeof(lang_value))) {
                int langId = atoi(lang_value);
                if (langId >= 0 && langId < Translation::Language::LanguageCount) {
                    Translation::setLanguage((Translation::Language)langId);
                    printf("[WebRadar] %s: %s\n",
                        Translation::get("Language changed to"),
                        Translation::getCurrentLanguageName());
                }
            }
            // 切换完成后重定向主页
            mg_http_reply(c, 302, "Location: /\r\n", "");
        }
        else if (strncmp(hm->uri.buf, "/maps/", 6) == 0) {
            radar->SendFile(c, hm);
        }
    }
}
void WebRadar::SendData(struct mg_connection* c) {
    std::ostringstream ss;

    // 没有有效地图时直接返回空结果
    if (!map->isinMatch) {
        static bool hasLoggedMapError = false;
        if (!hasLoggedMapError) {
            printf("[WebRadar] No valid map data\n");
            hasLoggedMapError = true;
        }
        ss << "{\"map\":\"unknown\",\"players\":[],\"spectators\":[]}";
        std::string json = ss.str();
        mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n", json.c_str());
        return;
    }


    const auto& spectators = spectator->GetSpectators();
    std::unordered_set<std::string> uniqueSpectators(spectators.begin(), spectators.end());


    ss << "{\"map\":\"" << map->MapName << "\",\"spectators\":[";

    bool firstSpectator = true;
    for (const auto& name : uniqueSpectators) {
        if (!firstSpectator) ss << ",";
        ss << "\"" << name << "\"";
        firstSpectator = false;
    }

    ss << "],\"players\":[";

    bool firstPlayer = true;
    auto playersCopy = entityManager.getPlayers();
    for (const auto& player : *playersCopy) {
        if (player->IsDead || player->Health <= 0 || !player->valid) continue;

        if (!firstPlayer) ss << ",";
        firstPlayer = false;

        ss << "{"
            << "\"x\":" << player->Origin.x << ","
            << "\"y\":" << player->Origin.y << ","
            << "\"name\":\"" << (player->Team == 97 ? "Bot" : (player->NickName.empty() ? "Player" : (player->IsPro ? "*PRO*" + player->NickName : player->NickName))) << "\","
            << "\"health\":" << player->Health << ","
            << "\"shield\":" << player->Shield << ","
            << "\"team\":" << player->Team << ","
            << "\"viewYaw\":" << player->ViewYaw << ","
            << "\"isVisible\":" << player->IsVisible << ","
            << "\"Knocked\":" << player->IsKnocked << ","
            << "\"isTeammate\":" << player->IsTeamMate << ","
            << "\"isLocal\":" << player->IsLocal << ","
            << "\"kills\":" << (player->Kills ? player->Kills : 0) << ","
            << "\"damage\":" << (player->Damages ? player->Damages : 0) << ","
            << "\"rankName\":\"" << (player->RankName.empty() ? "Unknown" : player->RankName) << "\","
            << "\"level\":" << player->Level << ","
            << "\"distance\":" << player->Distance2DToLocalPlayer << ","
            << "\"Dead\":" << player->IsDead << ","
            << "\"isPro\":" << player->IsPro
            << "}";
    }

    ss << "],\"currentLang\":" << Translation::getCurrentLanguage() << "}";

    // 调试时可打印 JSON
    //std::cout << "Generated JSON: " << ss.str() << std::endl;

    std::string json = ss.str();
    mg_http_reply(c, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n", json.c_str());
}

void WebRadar::Start() {
    running = true;

    serverThread = std::thread([this]() {
        printf("[WebRadar] Starting web radar server...\n");
        struct mg_connection* c = mg_http_listen(&mgr, "http://0.0.0.0:8080", EventHandler, this);
        if (c == nullptr) {
            printf("[WebRadar] Failed to start radar server\n");
            return;
        }

        printf("[WebRadar] Server started successfully. Please visit http://localhost:8080\n");

        while (running) {
            mg_mgr_poll(&mgr, 1);
        }
        });
}

void WebRadar::Stop() {
    running = false;
    if (serverThread.joinable()) {
        serverThread.join();
    }
    mg_mgr_free(&mgr);
    printf("[WebRadar] Server stopped\n");
}

// 生成 WebRadar 页面 HTML
void WebRadar::SendHtml(struct mg_connection* c) {
    // 当前语言 ID
    int currentLangId = Translation::getCurrentLanguage();

    // HTML 头部模板
    std::string htmlHead = R"(<!DOCTYPE html>
<html>
<head>
    <title>)" + std::string(Translation::get("Game Radar")) + R"(</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            background: #111;
            color: #fff;
            margin: 0;
            padding: 20px;
            font-family: Arial, sans-serif;
            min-height: 100vh;
            box-sizing: border-box;
            overflow: auto;
        }
        .radar-container {
            position: relative;
            margin: auto;
            overflow: hidden;
            max-height: 90vh;
            max-width: 90vw;
        }
        #radar {
            background: #000;
            border: 2px solid #333;
            display: block;
            margin: auto;
            cursor: grab;
        }
        .info-panel, .map-info, .spectator-info, .language-selector {
            position: fixed;
            background: rgba(0,0,0,0.8);
            padding: 10px;
            border-radius: 5px;
            z-index: 1000;
        }
        .info-panel {
            top: 10px;
            left: 10px;
        }
        .map-info {
            top: 10px;
            right: 10px;
        }
        .spectator-info {
            top: 70px;
            right: 10px;
        }
        .language-selector {
            top: 130px;
            right: 10px;
        }
        .spectator-info ul {
            list-style-type: none;
            padding: 0;
            margin: 0;
        }
        .spectator-info li {
            margin-bottom: 5px;
        }
        .checkbox-container {
            margin-top: 10px;
            color: #fff;
        }
        select {
            background: #333;
            color: #fff;
            border: 1px solid #555;
            padding: 5px;
            border-radius: 3px;
            width: 100%;
        }
    </style>
</head>
<body>
)";

    // 信息面板模板
    std::string infoPanel = R"(
    <div class="info-panel">
        <div>)" + std::string(Translation::get("Players")) + R"(: <span id="playerCount">0</span></div>
        <div>)" + std::string(Translation::get("Mouse Coordinates")) + R"(: <span id="mouseX">0</span>, <span id="mouseY">0</span></div>
        <div class="checkbox-container">
            <label><input type="checkbox" id="centerOnLocal"> )" + std::string(Translation::get("Center View")) + R"(</label>
        </div>
        <div class="checkbox-container">
            <label><input type="checkbox" id="showHealth"> )" + std::string(Translation::get("Show Health")) + R"(</label>
        </div>
        <div class="checkbox-container">
            <label><input type="checkbox" id="showName"> )" + std::string(Translation::get("Show Name")) + R"(</label>
        </div>
        <div class="checkbox-container">
            <label><input type="checkbox" id="showRank"> )" + std::string(Translation::get("Show Rank")) + R"(</label>
        </div>
        <div class="checkbox-container">
            <label><input type="checkbox" id="showKills"> )" + std::string(Translation::get("Show Kills & Damage")) + R"(</label>
        </div>
    </div>
)";

    // 地图信息区域
    std::string mapInfo = R"(
    <div class="map-info">
        <div>)" + std::string(Translation::get("Map")) + R"(: <span id="mapName">Unknown</span></div>
    </div>
)";

    // 观战者列表区域
    std::string spectatorInfo = R"(
    <div class="spectator-info">
        <div>)" + std::string(Translation::get("Total Spectators:")) + R"( <span id="spectatorCount">0</span></div>
        <ul id="spectatorList"></ul>
    </div>
)";

    std::string languageSelector =
        "    <div class=\"language-selector\">\n"
        "        <div>" + std::string(Translation::get("Language")) + ":</div>\n"
        "        <select id=\"languageSelect\" onchange=\"changeLanguage(this.value)\">\n"
        "            <option value=\"0\"" + (currentLangId == 0 ? " selected" : "") + ">English</option>\n"
        "            <option value=\"1\"" + (currentLangId == 1 ? " selected" : "") + ">��������</option>\n"
        "            <option value=\"2\"" + (currentLangId == 2 ? " selected" : "") + ">���w����</option>\n"
        "            <option value=\"3\"" + (currentLangId == 3 ? " selected" : "") + ">�ձ��Z</option>\n"
        "        </select>\n"
        "    </div>\n";

        // 雷达画布容器
        std::string radarContainer = R"(
    <div class="radar-container">
        <canvas id="radar"></canvas>
    </div>
)";

    // 注入 JavaScript 片段
    std::string translationsJs = R"(
    // JavaScript 字典：包含多语言文本
    let translations = {
        // 将 C++ 翻译结果写入 JS 对象
        "Players": ")" + std::string(Translation::get("Players")) + R"(",
        "Mouse Coordinates": ")" + std::string(Translation::get("Mouse Coordinates")) + R"(",
        "Center View": ")" + std::string(Translation::get("Center View")) + R"(",
        "Show Health": ")" + std::string(Translation::get("Show Health")) + R"(",
        "Show Name": ")" + std::string(Translation::get("Show Name")) + R"(",
        "Show Rank": ")" + std::string(Translation::get("Show Rank")) + R"(",
        "Show Kills & Damage": ")" + std::string(Translation::get("Show Kills & Damage")) + R"(",
        "Map": ")" + std::string(Translation::get("Map")) + R"(",
        "Total Spectators:": ")" + std::string(Translation::get("Total Spectators:")) + R"(",
        "Language": ")" + std::string(Translation::get("Language")) + R"("
    };
)";

    // JavaScript 主逻辑
    std::string jsCode = R"(
    function changeLanguage(langId) {
        window.location.href = "/setlang?lang=" + langId;
    }
    
    // 拖拽与缩放相关状态
    let isDragging = false;
    let lastMouseX = 0;
    let lastMouseY = 0;
    let offsetX = 0;
    let offsetY = 0;
    let scale = 1.0;
    let currentMap = null;
    let MIN_SCALE = 0.5;
    let MAX_SCALE = 2.0;
    const canvas = document.getElementById('radar');
    const ctx = canvas.getContext('2d');
    const mapImages = {};
    const DEG2RAD = Math.PI / 180;
    const RAD2DEG = 180 / Math.PI;

    let centerOnLocal = false;
    let isDraggingEnabled = true;
    let showHealth = false;
    let showName = false;
    let showRank = false;
    let showKills = false;
    let currentLang = )" + std::to_string(currentLangId) + R"(;

    document.getElementById('centerOnLocal').addEventListener('change', (e) => {
        centerOnLocal = e.target.checked;
        isDraggingEnabled = !centerOnLocal;
    });
    document.getElementById('showHealth').addEventListener('change', (e) => {
        showHealth = e.target.checked;
    });
    document.getElementById('showName').addEventListener('change', (e) => {
        showName = e.target.checked;
    });
    document.getElementById('showRank').addEventListener('change', (e) => {
        showRank = e.target.checked;
    });
    document.getElementById('showKills').addEventListener('change', (e) => {
        showKills = e.target.checked;
    });

    const MAP_CONFIG = {
        'mp_rr_arena_habitat': {
            x: 2048, y: 2048, scale: 0.04745,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_arena_habitat.png' }]
        },
        'mp_rr_divided_moon_mu1': {
            x: 2145, y: 2072, scale: 0.0476,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_divided_moon_mu1.png' }]
        },
        'mp_rr_canyonlands_staging_mu1': {
            x: 2048, y: 2048, scale: 0.04745,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: './maps/mp_rr_district.png' }]
        },
        'mp_rr_tropic_island_mu2': {
            x: 2026, y: 2089, scale: 0.04,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_tropic_island_mu2.png' }]
        },
        'mp_rr_freedm_skulltown': {
            x: 2427, y: 1990, scale: 0.2,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_freedm_skulltown.png' }]
        },
        'mp_rr_district': {
            x: 2048, y: 2048, scale: 0.04745,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_district.png' }]
        },
        'mp_rr_olympus_mu2': {
            x: 2365, y: 2185, scale: 0.0454,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_olympus_mu2.png' }]
        },
        'mp_rr_thunderdome': {
            x: 2050, y: 2050, scale: 0.2505,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_thunderdome.png' }]
        },
        'mp_rr_desertlands_hu': {
            x: 2050, y: 2050, scale: 0.0455,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_desertlands_hu.png' }]
        },
        'mp_rr_canyonlands_hu': {
            x: 1880, y: 2195, scale: 0.04995,
            offset: { x: 400, y: 400 },
            layers: [{ minHeight: -100, filename: '/maps/mp_rr_canyonlands_hu.png' }]
        }
    };
)";

    std::string jsCode2 = R"(
    function worldToScreen(worldX, worldY, config) {
        return {
            x: config.x + (worldX * config.scale),
            y: config.y - (worldY * config.scale)
        };
    }

    function screenToWorld(screenX, screenY, config) {
        return {
            x: (screenX - config.x) / config.scale,
            y: -(screenY - config.y) / config.scale
        };
    }

    function applyTransform() {
        ctx.setTransform(1, 0, 0, 1, 0, 0);
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        
        const currentWidth = canvas.width * scale;
        const currentHeight = canvas.height * scale;
        
        offsetX = Math.min(0, Math.max(canvas.width - currentWidth, offsetX));
        offsetY = Math.min(0, Math.max(canvas.height - currentHeight, offsetY));
        
        ctx.translate(offsetX, offsetY);
        ctx.scale(scale, scale);
    }

    function drawGrid() {
        ctx.strokeStyle = '#333333';
        ctx.lineWidth = 1;
        
        for(let i = 0; i < canvas.width; i += 50) {
            ctx.beginPath();
            ctx.moveTo(i, 0);
            ctx.lineTo(i, canvas.height);
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(0, i);
            ctx.lineTo(canvas.width, i);
            ctx.stroke();
        }
    }

    function drawViewDirection(x, y, yaw, directionLength, fovRadius) {
        const viewAngle = (-yaw) * DEG2RAD;
        const viewX = Math.cos(viewAngle);
        const viewY = Math.sin(viewAngle);
        
        ctx.beginPath();
        ctx.moveTo(x, y);
        ctx.lineTo(x + viewX * directionLength, y + viewY * directionLength);
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = Math.max(2 / scale, 1);
        ctx.stroke();

        ctx.beginPath();
        const fovAngle = 90;
        const startAngle = (-yaw - fovAngle/2) * DEG2RAD;
        const endAngle = (-yaw + fovAngle/2) * DEG2RAD;
        ctx.moveTo(x, y);
        ctx.arc(x, y, fovRadius, startAngle, endAngle);
        ctx.lineTo(x, y);
        ctx.closePath();
        ctx.fillStyle = 'rgba(255, 255, 255, 0.1)';
        ctx.fill();
    }
)";

    std::string jsCode3 = R"(
    function drawPlayerInfo(x, y, player, fontSize) {
        ctx.fillStyle = '#05f7df';
        ctx.font = `${fontSize}px Arial`;

        if (showName) {
            const nameText = `[Lv${player.level}] ${player.name}`;
            const textWidth = ctx.measureText(nameText).width;
            ctx.fillText(nameText, x - textWidth / 2, y - 15);
        }

        let rankYOffset = 0;

        if (showKills && player.kills >= 0 && player.damage >= 0) {
            const spacing = Math.max(15 / scale, 8);
            const offsetX = 10 / scale;

            ctx.fillStyle = '#ffd700';
            ctx.font = `${Math.max(10 / scale, 10)}px Arial`;
            ctx.fillText(`K: ${player.kills}`, x - spacing - offsetX, y + spacing);

            ctx.fillStyle = '#ff0000';
            ctx.fillText(`D: ${player.damage}`, x + spacing - offsetX, y + spacing);
            
            rankYOffset = Math.max(30 / scale, 15);
        } else {
            rankYOffset = 15;
        }

        if (showRank && player.rankName) {
            ctx.fillStyle = getRankColor(player.rankName);
            ctx.font = `${Math.max(10 / scale, 10)}px Arial`;

            const textWidth = ctx.measureText(player.rankName).width;
            ctx.fillText(player.rankName, x - textWidth / 2, y + rankYOffset);
        }

        if (showHealth && player.health > 0) {
            const barHeight = Math.max(25 / scale, 25);
            const barWidth = Math.max(3 / scale, 3);
            const barX = x - 15;
            const barY = y + barHeight / 2;

            ctx.fillStyle = '#333333';
            ctx.fillRect(barX, y - barHeight / 2, barWidth, barHeight);

            const healthHeight = (player.health / 100) * barHeight;
            ctx.fillStyle = '#ff0000';
            ctx.fillRect(barX, barY - healthHeight, barWidth, healthHeight);

            if (player.shield > 0) {
                const shieldBarX = barX - barWidth - 1;
                ctx.fillStyle = '#333333';
                ctx.fillRect(shieldBarX, y - barHeight / 2, barWidth, barHeight);

                const shieldHeight = (player.shield / 100) * barHeight;
                ctx.fillStyle = '#00aaff';
                ctx.fillRect(shieldBarX, barY - shieldHeight, barWidth, shieldHeight);
            }
        }
    }

    function getRankColor(rankName) {
        const colors = {
            "ͭ": "#cd7f32",
            "": "#c0c0c0",
            "ƽ": "#ffd700",
            "": "#00b4d8",
            "ʯ": "#b9f2ff",
            "ʦ": "#a020f0",
            "ɱ": "#ff0000"
        };
        return colors[rankName] || "#ffffff";
    }
)";

    std::string jsCode4 = R"(
    function drawPlayer(player, config) {
        if (player.health <= 0 || player.Dead) return;
        const pos = worldToScreen(player.x, player.y, config);
        
        const baseSize = 6;
        const adjustedSize = Math.max(baseSize / scale, baseSize);
        
        ctx.beginPath();
        ctx.arc(pos.x, pos.y, adjustedSize, 0, Math.PI * 2);
        ctx.fillStyle = player.isLocal ? '#00ffff' :
                       player.isTeammate ? '#201000' :
                       player.isPro ? '#00ffff' :
                       player.isVisible ? '#00ff00' : '#ff0000';
        if (player.Knocked) {
            ctx.fillStyle = '#0055ff';
        }
        ctx.fill();
        ctx.stroke();

        const directionLength = Math.max(20 / scale, 20);
        const fovRadius = Math.max(30 / scale, 30);
        drawViewDirection(pos.x, pos.y, player.viewYaw, directionLength, fovRadius);
        
        const fontSize = Math.max(12 / scale, 12);
        drawPlayerInfo(pos.x, pos.y, player, fontSize);
    }

    function drawMap(mapImage) {
        canvas.width = mapImage.width;
        canvas.height = mapImage.height;
        ctx.drawImage(mapImage, 0, 0, mapImage.width, mapImage.height);
        drawGrid();
    }

    function preloadMapImages() {
        Object.values(MAP_CONFIG).forEach(mapConfig => {
            const img = new Image();
            const filename = mapConfig.layers[0].filename;
            
            console.log(`[Maps] Trying to load: ${filename}`);
            
            img.onload = () => {
                console.log(`[Maps] Successfully loaded: ${filename}`);
                console.log(`[Maps] Image dimensions: ${img.width}x${img.height}`);
                mapImages[filename] = img;
                
                if (Object.keys(mapImages).length === 1) {
                    canvas.width = img.width;
                    canvas.height = img.height;
                }
            };
            
            img.onerror = (error) => {
                console.error(`[Maps] Failed to load: ${filename}`, error);
            };

            img.src = filename;
        });
    }
)";

    std::string jsCode5 = R"(
    function updateBoundaries() {
        const scaledWidth = canvas.width * scale;
        const scaledHeight = canvas.height * scale;
        
        const minScaleX = window.innerWidth / canvas.width;
        const minScaleY = window.innerHeight / canvas.height;
        MIN_SCALE = Math.min(minScaleX, minScaleY) * 0.5;
        
        offsetX = Math.min(0, Math.max(window.innerWidth - scaledWidth, offsetX));
        offsetY = Math.min(0, Math.max(window.innerHeight - scaledHeight, offsetY));
    }

    function updateRadar() {
        fetch('/data')
            .then(response => response.json())
            .then(data => {
                const playerCount = data.players ? data.players.length : 0;
                document.getElementById('playerCount').textContent = playerCount;

                if (data.currentLang !== undefined && data.currentLang !== currentLang) {
                    currentLang = data.currentLang;
                    document.getElementById('languageSelect').value = currentLang.toString();
                    console.log("Language synchronized with server: " + currentLang);
                }

                currentMap = data.map;
                const mapConfig = MAP_CONFIG[data.map];
                if (!mapConfig) {
                    console.log("Map config not found for:", data.map);
                    return;
                }

                document.getElementById('mapName').textContent = data.map;

                const spectatorList = document.getElementById('spectatorList');
                spectatorList.innerHTML = '';

                const spectators = data.spectators || [];
                document.getElementById('spectatorCount').textContent = spectators.length;

                spectators.forEach(spectator => {
                    const listItem = document.createElement('li');
                    listItem.textContent = spectator;
                    spectatorList.appendChild(listItem);
                });

                const mapImage = mapImages[mapConfig.layers[0].filename];
                if (!mapImage || !mapImage.complete) return;

                let localPlayer = null;
                data.players?.forEach(p => {
                    if (p.isLocal) localPlayer = p;
                });

                updateBoundaries();

                ctx.clearRect(0, 0, canvas.width * 1.2, canvas.height);
                ctx.setTransform(scale, 0, 0, scale, offsetX, offsetY);

                ctx.drawImage(mapImage, 0, 0, mapImage.width, mapImage.height);

                drawGrid();

                if (centerOnLocal && localPlayer) {
                    const pos = worldToScreen(localPlayer.x, localPlayer.y, mapConfig);
                    offsetX = window.innerWidth/2 - pos.x * scale;
                    offsetY = window.innerHeight/2 - pos.y * scale;
                }

                if(data.players && data.players.length > 0) {
                    data.players.forEach(player => {
                        drawPlayer(player, mapConfig);
                    });
                }
            })
            .catch(error => console.error('Radar update error:', error));
    }
)";

    std::string jsCode6 = R"(
    canvas.addEventListener('mousedown', (e) => {
        isDragging = true;
        lastMouseX = e.clientX;
        lastMouseY = e.clientY;
        canvas.style.cursor = 'grabbing';
    });

    canvas.addEventListener('mouseup', () => {
        isDragging = false;
        canvas.style.cursor = 'grab';
    });

    canvas.addEventListener('mouseleave', () => {
        isDragging = false;
        canvas.style.cursor = 'grab';
    });

    canvas.addEventListener('wheel', (e) => {
        e.preventDefault();

        const rect = canvas.getBoundingClientRect();
        const mouseX = e.clientX - rect.left;
        const mouseY = e.clientY - rect.top;

        const viewportX = (mouseX - offsetX) / scale;
        const viewportY = (mouseY - offsetY) / scale;

        const newScale = scale * (e.deltaY < 0 ? 1.1 : 0.9);

        if (newScale >= MIN_SCALE && newScale <= MAX_SCALE) {
            scale = newScale;

            offsetX = mouseX - viewportX * scale;
            offsetY = mouseY - viewportY * scale;

            updateBoundaries();
        }
    });

    canvas.addEventListener('mousemove', (e) => {
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        if (isDraggingEnabled && isDragging) {
            const deltaX = e.clientX - lastMouseX;
            const deltaY = e.clientY - lastMouseY;

            offsetX += deltaX;
            offsetY += deltaY;

            updateBoundaries();

            lastMouseX = e.clientX;
            lastMouseY = e.clientY;
        }

        if (currentMap && MAP_CONFIG[currentMap]) {
            const viewportX = (x - offsetX) / scale;
            const viewportY = (y - offsetY) / scale;
            const worldPos = screenToWorld(viewportX, viewportY, MAP_CONFIG[currentMap]);
            document.getElementById('mouseX').textContent = Math.round(worldPos.x);
            document.getElementById('mouseY').textContent = Math.round(worldPos.y);
        }
    });

    preloadMapImages();
    setInterval(updateRadar, 1000 / 30);
)";

    // 拼接完整的 HTML 与 JavaScript
    std::string completeHtml = htmlHead;
    completeHtml += infoPanel;
    completeHtml += mapInfo;
    completeHtml += spectatorInfo;
    completeHtml += languageSelector;
    completeHtml += radarContainer;
    completeHtml += "<script>\n" + translationsJs + jsCode + jsCode2 + jsCode3 + jsCode4 + jsCode5 + jsCode6 + "\n</script>\n";
    completeHtml += "</body>\n</html>";

    // 将 HTML 返回给浏览器
    mg_http_reply(c, 200, "Content-Type: text/html; charset=utf-8\r\n", "%s", completeHtml.c_str());
}
