#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#pragma execution_character_set("utf-8")
// 极简翻译系统
class Translation {
public:
    // 语言索引
    enum Language {
        English = 0,
        SimplifiedChinese = 1,
        TraditionalChinese = 2,
        Japanese = 3,
        LanguageCount
    };

private:
    // 当前语言
    static inline Language currentLanguage = Translation::Language::English;

    // 翻译表: key -> [英文, 简中, 繁中, 日文]
    static inline std::unordered_map<std::string, std::vector<std::string>> translations;

    // 初始化所有翻译
   // 翻译系统初始化函数
    static void initTranslations() {
        // 基础UI元素
        translations["Settings"] = { "Settings", "设置", "設置", "設定" };
        translations["Language"] = { "Language", "语言", "語言", "言語" };
        translations["Interface"] = { "Interface", "界面", "界面", "インターフェース" };
        translations["Other Settings"] = { "Other Settings", "其他设置", "其他設置", "その他の設定" };
        translations["Language Settings"] = { "Language Settings", "语言设置", "語言設置", "言語設定" };
        translations["Selected Language: "] = { "Selected Language", "已选语言", "已選語言", "選択言語" };
        translations["Interface Settings"] = { "Interface Settings", "界面设置", "界面設置", "インターフェース設定" };
        translations["Show FPS"] = { "Show FPS", "显示帧率", "顯示幀率", "FPS表示" };
        translations["Theme: "] = { "Theme", "主题", "主題", "テーマ" };
        translations["Dark"] = { "Dark", "暗色", "暗色", "ダーク" };
        translations["Light"] = { "Light", "亮色", "亮色", "ライト" };
        translations["Classic"] = { "Classic", "经典", "經典", "クラシック" };
        translations["Custom"] = { "Custom", "自定义", "自定義", "カスタム" };
        translations["Enable Logging"] = { "Enable Logging", "启用日志", "啟用日誌", "ログを有効にする" };
        translations["Auto Update"] = { "Auto Update", "自动更新", "自動更新", "自動更新" };
        translations["Apply"] = { "Apply", "应用", "應用", "適用" };
        translations["Save"] = { "Save", "保存", "保存", "保存" };
        translations["Load"] = { "Load", "加载", "加載", "ロード" };
        translations["Reset"] = { "Reset", "重置", "重置", "リセット" };
        translations["Enabled"] = { "Enabled", "启用", "啟用", "有効" };
        translations["OK"] = { "OK", "确定", "確定", "OK" };

        // 导航项
        translations["Aimbot"] = { "Aimbot", "自动瞄准", "自動瞄準", "エイムボット" };
        translations["ESP"] = { "ESP", "透视", "透視", "ESP" };
        translations["Glow"] = { "Glow", "发光", "發光", "グロー" };
        translations["Triggerbot"] = { "Triggerbot", "扳机机器人", "扳機機器人", "トリガーボット" };
        translations["Misc"] = { "Misc", "杂项", "雜項", "その他" };
        translations["Config"] = { "Config", "配置", "配置", "設定" };

        // 配置管理相关
        translations["Configuration Management"] = { "Configuration Management", "配置管理", "配置管理", "設定管理" };
        translations["Available Configurations"] = { "Available Configurations", "可用配置", "可用配置", "利用可能な設定" };
        translations["Refresh List"] = { "Refresh List", "刷新列表", "刷新列表", "リスト更新" };
        translations["No configuration files found"] = { "No configuration files found", "未找到配置文件", "未找到配置文件", "設定ファイルが見つかりません" };
        translations["Create New Configuration"] = { "Create New Configuration", "创建新配置", "創建新配置", "新しい設定を作成" };
        translations["Configuration Name"] = { "Configuration Name", "配置名称", "配置名稱", "設定名" };
        translations["Save as New Configuration"] = { "Save as New Configuration", "另存为新配置", "另存為新配置", "新しい設定として保存" };
        translations["Configuration Actions"] = { "Configuration Actions", "配置操作", "配置操作", "設定アクション" };
        translations["Load Default"] = { "Load Default", "加载默认配置", "加載默認配置", "デフォルト設定を読み込む" };
        translations["Selected Configuration Actions"] = { "Selected Configuration Actions", "已选配置操作", "已選配置操作", "選択された設定のアクション" };
        translations["Load Selected Configuration"] = { "Load Selected Configuration", "加载已选配置", "加載已選配置", "選択した設定を読み込む" };
        translations["Save Configuration"] = { "Save Configuration", "保存配置", "保存配置", "設定を保存" };
        translations["Load Configuration"] = { "Load Configuration", "加载配置", "加載配置", "設定を読み込む" };
        translations["Save as Default"] = { "Save as Default", "设为默认", "設為默認", "デフォルトとして保存" };
        translations["Delete Configuration"] = { "Delete Configuration", "删除配置", "刪除配置", "設定を削除" };
        translations["Success"] = { "Success", "成功", "成功", "成功" };
        translations["Operation Completed Successfully!"] = { "Operation Completed Successfully!", "操作成功完成！", "操作成功完成！", "操作が正常に完了しました！" };
        translations["Failed"] = { "Failed", "失败", "失敗", "失敗" };
        translations["Operation Failed. Please Check Error Logs!"] = { "Operation Failed. Please Check Error Logs!", "操作失败。请检查错误日志！", "操作失敗。請檢查錯誤日誌！", "操作に失敗しました。エラーログを確認してください！" };
        translations["Error"] = { "Error", "错误", "錯誤", "エラー" };
        translations["Please Enter a Configuration Name!"] = { "Please Enter a Configuration Name!", "请输入配置名称！", "請輸入配置名稱！", "設定名を入力してください！" };

        // Aimbot相关
        translations["AIMBOT SETTINGS"] = { "AIMBOT SETTINGS", "自动瞄准设置", "自動瞄準設置", "エイムボット設定" };
        translations["Global"] = { "Global", "全局", "全局", "グローバル" };
        translations["Hitboxes"] = { "Hitboxes", "命中框", "命中框", "ヒットボックス" };
        translations["Enable Aimbot"] = { "Enable Aimbot", "启用自动瞄准", "啟用自動瞄準", "エイムボットを有効化" };
        translations["Basic Settings"] = { "Basic Settings", "基本设置", "基本設置", "基本設定" };
        translations["Aim Type"] = { "Aim Type", "瞄准类型", "瞄準類型", "エイムタイプ" };
        translations["Memory( Dangerous! )"] = { "Memory( Dangerous! )", "内存（危险！）", "內存（危險！）", "メモリ（危険！）" };
        translations["Kmbox"] = { "KMBOX", "KMBOX", "KMBOX", "KMBOX" };
        translations["Use memory aim at your own risk!\nMemory aim is bad, not suggest!"] = { "Use memory aim at your own risk!\nMemory aim is bad, not suggest!", "使用内存瞄准风险自负！\n内存瞄准不稳定，不建议使用！", "使用內存瞄準風險自負！\n內存瞄準不穩定，不建議使用！", "メモリエイムは自己責任で使用してください！\nメモリエイムは不安定で、お勧めしません！" };
        translations["Normal Mode"] = { "Normal Mode", "普通模式", "普通模式", "通常モード" };
        translations["FOV##Normal"] = { "FOV", "视野", "視野", "視野" };
        translations["Smooth##Normal"] = { "Smooth", "平滑度", "平滑度", "スムース" };
        translations["Scoped Mode"] = { "Scoped Mode", "瞄准镜模式", "瞄準鏡模式", "スコープモード" };
        translations["FOV##Scoped"] = { "FOV", "视野", "視野", "視野" };
        translations["Smooth##Scoped"] = { "Smooth", "平滑度", "平滑度", "スムース" };
        translations["Key Bindings"] = { "Key Bindings", "按键绑定", "按鍵綁定", "キーバインド" };
        translations["Primary Aim"] = { "Primary Aim", "主要瞄准", "主要瞄準", "プライマリーエイム" };
        translations["Secondary Aim"] = { "Secondary Aim", "次要瞄准", "次要瞄準", "セカンダリエイム" };
        translations["Force Head Aim"] = { "Force Head Aim", "强制瞄头", "強制瞄頭", "強制ヘッドエイム" };
        translations["Visual Settings"] = { "Visual Settings", "视觉设置", "視覺設置", "視覚設定" };
        translations["Show FOV Circle"] = { "Show FOV Circle", "显示视野圈", "顯示視野圈", "視野円を表示" };
        translations["FOV Circle Thickness"] = { "FOV Circle Thickness", "视野圈粗细", "視野圈粗細", "視野円の太さ" };
        translations["Normal Color"] = { "Normal Color", "正常颜色", "正常顏色", "通常色" };
        translations["Locked Color"] = { "Locked Color", "锁定颜色", "鎖定顏色", "ロック色" };
        translations["Target Selection"] = { "Target Selection", "目标选择", "目標選擇", "ターゲット選択" };
        translations["Target Types"] = { "Target Types", "目标类型", "目標類型", "ターゲットタイプ" };
        translations["Target Enemies"] = { "Target Enemies", "瞄准敌人", "瞄準敵人", "敵をターゲット" };
        translations["Target Teammates"] = { "Target Teammates", "瞄准队友", "瞄準隊友", "味方をターゲット" };
        translations["Target Bots"] = { "Target Bots", "瞄准机器人", "瞄準機器人", "ボットをターゲット" };
        translations["Priority Mode"] = { "Priority Mode", "优先级模式", "優先級模式", "優先モード" };
        translations["Distance"] = { "Distance", "距离", "距離", "距離" };
        translations["FOV"] = { "FOV", "视野", "視野", "視野" };
        translations["Health"] = { "Health", "生命值", "生命值", "体力" };
        translations["Target Filtering"] = { "Target Filtering", "目标过滤", "目標過濾", "ターゲットフィルタリング" };
        translations["Visible Only"] = { "Visible Only", "仅可见目标", "僅可見目標", "可視ターゲットのみ" };
        translations["Ignore Knocked Down"] = { "Ignore Knocked Down", "忽略已击倒", "忽略已擊倒", "ダウン敵を無視" };
        translations["Humanization Settings"] = { "Humanization Settings", "人性化设置", "人性化設置", "人間らしさ設定" };
        translations["Enable Humanization"] = { "Enable Humanization", "启用人性化", "啟用人性化", "人間らしさを有効化" };
        translations["Adjust Type"] = { "Adjust Type", "调整类型", "調整類型", "調整タイプ" };
        translations["All"] = { "All", "全部", "全部", "すべて" };
        translations["Only on aimed at"] = { "Only on aimed at", "仅当被瞄准", "僅當被瞄準", "狙われた時のみ" };
        translations["Only on shoot"] = { "Only on shoot", "仅当射击", "僅當射擊", "撃った時のみ" };
        translations["Micro-Adjustment Scale"] = { "Micro-Adjustment Scale", "微调比例", "微調比例", "微調整スケール" };
        translations["Adjustment Directions"] = { "Adjustment Directions", "调整方向", "調整方向", "調整方向" };
        translations["Horizontal (Yaw)"] = { "Horizontal (Yaw)", "水平(偏航)", "水平(偏航)", "水平(ヨー)" };
        translations["Vertical (Pitch)"] = { "Vertical (Pitch)", "垂直(俯仰)", "垂直(俯仰)", "垂直(ピッチ)" };
        translations["Warning: All directions disabled!"] = { "Warning: All directions disabled!", "警告：所有方向都被禁用！", "警告：所有方向都被禁用！", "警告：すべての方向が無効です！" };
        translations["Non-Linear Path Strength"] = { "Non-Linear Path Strength", "非线性路径强度", "非線性路徑強度", "非線形パス強度" };
        translations["Time Variation"] = { "Time Variation", "时间变化", "時間變化", "時間変動" };
        translations["Show Lock Indicator"] = { "Show Lock Indicator", "显示锁定指示器", "顯示鎖定指示器", "ロック表示を表示" };
        translations["Indicator Size"] = { "Indicator Size", "指示器大小", "指示器大小", "表示サイズ" };
        translations["Indicator Color"] = { "Indicator Color", "指示器颜色", "指示器顏色", "表示色" };
        translations["Advanced Settings"] = { "Advanced Settings", "高级设置", "高級設置", "詳細設定" };
        translations["Recoil Compensation"] = { "Recoil Compensation", "后坐力补偿", "後坐力補償", "反動補正" };
        translations["Vertical"] = { "Vertical", "垂直", "垂直", "垂直" };
        translations["Horizontal"] = { "Horizontal", "水平", "水平", "水平" };
        translations["Scoped Vertical"] = { "Scoped Vertical", "瞄准镜垂直", "瞄準鏡垂直", "スコープ垂直" };
        translations["Scoped Horizontal"] = { "Scoped Horizontal", "瞄准镜水平", "瞄準鏡水平", "スコープ水平" };
        translations["Smoothing Settings"] = { "Smoothing Settings", "平滑设置", "平滑設置", "スムース設定" };
        translations["SwitchSmoothScale"] = { "SwitchSmoothScale", "切换平滑比例", "切換平滑比例", "切替スムーススケール" };
        translations["ShootingSmooth"] = { "ShootingSmooth", "射击平滑", "射擊平滑", "射撃スムース" };
        translations["Prediction Settings"] = { "Prediction Settings", "预测设置", "預測設置", "予測設定" };
        translations["Enable Prediction"] = { "Enable Prediction", "启用预测", "啟用預測", "予測を有効化" };
        translations["PredictionScale"] = { "PredictionScale", "预测比例", "預測比例", "予測スケール" };
        translations["Show Prediction Point"] = { "Show Prediction Point", "显示预测点", "顯示預測點", "予測点を表示" };
        translations["Hitbox Selection"] = { "Hitbox Selection", "命中框选择", "命中框選擇", "ヒットボックス選択" };
        translations["Auto Select Best Bone"] = { "Auto Select Best Bone", "自动选择最佳骨骼", "自動選擇最佳骨骼", "最適な骨を自動選択" };
        translations["Target Bone"] = { "Target Bone", "目标骨骼", "目標骨骼", "ターゲット骨" };
        translations["Head"] = { "Head", "头部", "頭部", "頭" };
        translations["Neck"] = { "Neck", "颈部", "頸部", "首" };
        translations["Upper Chest"] = { "Upper Chest", "上胸", "上胸", "上胸" };
        translations["Lower Chest"] = { "Lower Chest", "下胸", "下胸", "下胸" };
        translations["Stomach"] = { "Stomach", "腹部", "腹部", "腹" };
        translations["Pelvis"] = { "Pelvis", "骨盆", "骨盆", "骨盤" };
        translations["Target Specific Settings"] = { "Target Specific Settings", "目标特定设置", "目標特定設置", "ターゲット固有設定" };
        translations["Target Type"] = { "Target Type", "目标类型", "目標類型", "ターゲットタイプ" };
        translations["All Targets"] = { "All Targets", "所有目标", "所有目標", "すべてのターゲット" };
        translations["No Shield Players"] = { "No Shield Players", "无护盾玩家", "無護盾玩家", "シールドなしプレイヤー" };
        translations["Shield Players"] = { "Shield Players", "有护盾玩家", "有護盾玩家", "シールドありプレイヤー" };
        translations["Default Bone for"] = { "Default Bone for", "默认骨骼为", "默認骨骼為", "デフォルト骨：" };
        translations["Hitbox Visualization"] = { "Hitbox Visualization", "命中框可视化", "命中框可視化", "ヒットボックス可視化" };

        // ESP相关
        translations["ESP SETTINGS"] = { "ESP SETTINGS", "透视设置", "透視設置", "ESP設定" };
        translations["Enemy"] = { "Enemy", "敌人", "敵人", "敵" };
        translations["Teammate"] = { "Teammate", "队友", "隊友", "味方" };
        translations["Bot"] = { "Bot", "机器人", "機器人", "ボット" };
        translations["Item"] = { "Item", "物品", "物品", "アイテム" };
        translations["Categories"] = { "Categories", "分类", "分類", "カテゴリ" };
        translations["Offscreen"] = { "Offscreen", "屏幕外", "屏幕外", "画面外" };
        translations["Enable ESP"] = { "Enable ESP", "启用透视", "啟用透視", "ESPを有効化" };
        translations["ESP Categories"] = { "ESP Categories", "透视分类", "透視分類", "ESPカテゴリ" };
        translations["Enable Enemy ESP"] = { "Enable Enemy ESP", "启用敌人透视", "啟用敵人透視", "敵ESPを有効化" };
        translations["Enable Teammate ESP"] = { "Enable Teammate ESP", "启用队友透视", "啟用隊友透視", "味方ESPを有効化" };
        translations["Enable Bot ESP"] = { "Enable Bot ESP", "启用机器人透视", "啟用機器人透視", "ボットESPを有効化" };
        translations["Hold Mode"] = { "Hold Mode", "按住模式", "按住模式", "ホールドモード" };
        translations["Hold Mode: ESP is active only while the key is held down"] = { "Hold Mode: ESP is active only while the key is held down", "按住模式：只有在按住按键时透视才会激活", "按住模式：只有在按住按鍵時透視才會激活", "ホールドモード：キーを押している間のみESPが有効" };
        translations["Toggle Mode: Press the key to toggle ESP on/off"] = { "Toggle Mode: Press the key to toggle ESP on/off", "切换模式：按下按键切换透视开关", "切換模式：按下按鍵切換透視開關", "トグルモード：キーを押してESPのオン/オフを切り替える" };
        translations["Hold Key"] = { "Hold Key", "按住键", "按住鍵", "ホールドキー" };
        translations["Toggle Key"] = { "Toggle Key", "切换键", "切換鍵", "トグルキー" };
        translations["Display Settings"] = { "Display Settings", "显示设置", "顯示設置", "表示設定" };
        translations["Show Name"] = { "Show Name", "显示名称", "顯示名稱", "名前を表示" };
        translations["Show Distance"] = { "Show Distance", "显示距离", "顯示距離", "距離を表示" };
        translations["Show Level"] = { "Show Level", "显示等级", "顯示等級", "レベルを表示" };
        translations["Show Rank"] = { "Show Rank", "显示段位", "顯示段位", "ランクを表示" };
        translations["Show Weapon"] = { "Show Weapon", "显示武器", "顯示武器", "武器を表示" };
        translations["Show Health Bar"] = { "Show Health Bar", "显示生命条", "顯示生命條", "体力バーを表示" };
        translations["Show Skeleton"] = { "Show Skeleton", "显示骨架", "顯示骨架", "スケルトンを表示" };
        translations["Box Style"] = { "Box Style", "方框样式", "方框樣式", "ボックススタイル" };
        translations["None"] = { "None", "无", "無", "なし" };
        translations["2D Box"] = { "2D Box", "2D方框", "2D方框", "2Dボックス" };
        translations["Corner Box"] = { "Corner Box", "角方框", "角方框", "コーナーボックス" };
        translations["Health Bar Style"] = { "Health Bar Style", "生命条样式", "生命條樣式", "体力バースタイル" };
        translations["Seer"] = { "Seer", "预言者", "預言者", "シーア" };
        translations["Classic"] = { "Classic", "经典", "經典", "クラシック" };
        translations["Custom"] = { "Custom", "自定义", "自定義", "カスタム" };
        translations["Distance Settings"] = { "Distance Settings", "距离设置", "距離設置", "距離設定" };
        translations["Max Distance"] = { "Max Distance", "最大距离", "最大距離", "最大距離" };
        translations["Near Distance"] = { "Near Distance", "近距离", "近距離", "近距離" };
        translations["Color Settings"] = { "Color Settings", "颜色设置", "顏色設置", "色設定" };
        translations["Color By Health"] = { "Color By Health", "按生命值着色", "按生命值著色", "体力で色分け" };
        translations["Visible Color"] = { "Visible Color", "可见颜色", "可見顏色", "可視色" };
        translations["Invisible Color"] = { "Invisible Color", "不可见颜色", "不可見顏色", "不可視色" };
        translations["Pro Player Color"] = { "Pro Player Color", "专业玩家颜色", "專業玩家顏色", "プロプレイヤー色" };
        translations["Text Color"] = { "Text Color", "文本颜色", "文本顏色", "テキスト色" };
        translations["Enable Item ESP"] = { "Enable Item ESP", "启用物品透视", "啟用物品透視", "アイテムESPを有効化" };
        translations["Global Display Settings"] = { "Global Display Settings", "全局显示设置", "全局顯示設置", "グローバル表示設定" };
        translations["Show All Names"] = { "Show All Names", "显示所有名称", "顯示所有名稱", "すべての名前を表示" };
        translations["Show All Distances"] = { "Show All Distances", "显示所有距离", "顯示所有距離", "すべての距離を表示" };
        translations["Show All Icons"] = { "Show All Icons", "显示所有图标", "顯示所有圖標", "すべてのアイコンを表示" };
        translations["Enable Animation"] = { "Enable Animation", "启用动画", "啟用動畫", "アニメーションを有効化" };
        translations["Animation helps prevent screen clutter near crosshair"] = { "Animation helps prevent screen clutter near crosshair", "动画有助于防止准星附近的屏幕杂乱", "動畫有助於防止準星附近的屏幕雜亂", "アニメーションはクロスヘア付近の画面の乱雑さを防ぐのに役立ちます" };
        translations["Items fade in based on distance from screen center"] = { "Items fade in based on distance from screen center", "物品根据与屏幕中心的距离渐变显示", "物品根據與屏幕中心的距離漸變顯示", "アイテムは画面中央からの距離に基づいてフェードインします" };
        translations["This prevents cluttering around your aim point"] = { "This prevents cluttering around your aim point", "这可以防止瞄准点周围的杂乱", "這可以防止瞄準點周圍的雜亂", "これにより、照準点周りの乱雑さを防ぎます" };
        translations["Display Style"] = { "Display Style", "显示样式", "顯示樣式", "表示スタイル" };
        translations["Style"] = { "Style", "样式", "樣式", "スタイル" };
        translations["Text Only"] = { "Text Only", "仅文本", "僅文本", "テキストのみ" };
        translations["Icon Only"] = { "Icon Only", "仅图标", "僅圖標", "アイコンのみ" };
        translations["Text and Icon"] = { "Text and Icon", "文本和图标", "文本和圖標", "テキストとアイコン" };
        translations["Maximum Distance"] = { "Maximum Distance", "最大距离", "最大距離", "最大距離" };
        translations["Animation Settings"] = { "Animation Settings", "动画设置", "動畫設置", "アニメーション設定" };
        translations["Controls fade-in area size around crosshair"] = { "Controls fade-in area size around crosshair", "控制准星周围渐变区域大小", "控制準星周圍漸變區域大小", "クロスヘア周辺のフェードイン領域サイズを制御します" };
        translations["Scale"] = { "Scale", "比例", "比例", "スケール" };
        translations["Controls fade threshold - lower values show more items near crosshair"] = { "Controls fade threshold - lower values show more items near crosshair", "控制渐变阈值 - 较低的值在准星附近显示更多物品", "控制漸變閾值 - 較低的值在準星附近顯示更多物品", "フェードしきい値を制御 - 値が低いほどクロスヘア付近に表示されるアイテムが多くなります" };
        translations["Enable Offscreen Indicators"] = { "Enable Offscreen Indicators", "启用屏幕外指示器", "啟用屏幕外指示器", "画面外インジケーターを有効化" };
        translations["Indicator Settings"] = { "Indicator Settings", "指示器设置", "指示器設置", "インジケーター設定" };
        translations["Only Enemies"] = { "Only Enemies", "仅敌人", "僅敵人", "敵のみ" };
        translations["Indicator Radius"] = { "Indicator Radius", "指示器半径", "指示器半徑", "インジケーター半径" };
        translations["Controls how far from screen center the indicators appear"] = { "Controls how far from screen center the indicators appear", "控制指示器与屏幕中心的距离", "控制指示器與屏幕中心的距離", "インジケーターが画面中央からどれだけ離れて表示されるかを制御します" };
        translations["Value is percentage of screen size"] = { "Value is percentage of screen size", "值为屏幕大小的百分比", "值為屏幕大小的百分比", "値は画面サイズの割合です" };
        translations["Arrow Size"] = { "Arrow Size", "箭头大小", "箭頭大小", "矢印サイズ" };
        translations["Size of the triangle indicator"] = { "Size of the triangle indicator", "三角形指示器的大小", "三角形指示器的大小", "三角形インジケーターのサイズ" };
        translations["Arrow Angle"] = { "Arrow Angle", "箭头角度", "箭頭角度", "矢印角度" };
        translations["Controls how wide the arrow appears"] = { "Controls how wide the arrow appears", "控制箭头的宽度", "控制箭頭的寬度", "矢印の幅を制御します" };
        translations["Higher values = wider arrow"] = { "Higher values = wider arrow", "值越高 = 箭头越宽", "值越高 = 箭頭越寬", "値が高いほど = 矢印が広くなります" };
        translations["Dynamic Radius"] = { "Dynamic Radius", "动态半径", "動態半徑", "動的半径" };
        translations["Indicator radius changes based on enemy distance"] = { "Indicator radius changes based on enemy distance", "指示器半径根据敌人距离变化", "指示器半徑根據敵人距離變化", "インジケーター半径は敵の距離に基づいて変化します" };
        translations["Closer enemies have larger radius indicators"] = { "Closer enemies have larger radius indicators", "越近的敌人有越大的半径指示器", "越近的敵人有越大的半徑指示器", "近い敵ほど大きな半径インジケーターを持ちます" };

        // Glow相关
        translations["GLOW SETTINGS"] = { "GLOW SETTINGS", "发光设置", "發光設置", "グロー設定" };
        translations["Player Setting"] = { "Players", "玩家", "玩家", "プレイヤー" };
        translations["Items"] = { "Items", "物品", "物品", "アイテム" };
        translations["Global Enable"] = { "Global Enable", "全局启用", "全局啟用", "グローバル有効" };
        translations["Glow Categories"] = { "Glow Categories", "发光分类", "發光分類", "グローカテゴリ" };
        translations["Ignore Teammates"] = { "Ignore Teammates", "忽略队友", "忽略隊友", "味方を無視" };
        translations["Ignore Bots"] = { "Ignore Bots", "忽略机器人", "忽略機器人", "ボットを無視" };
        translations["Enable Player Glow"] = { "Enable Player Glow", "启用玩家发光", "啟用玩家發光", "プレイヤーグローを有効化" };
        translations["Glow Settings"] = { "Glow Settings", "发光设置", "發光設置", "グロー設定" };
        translations["Based On Shield"] = { "Based On Shield", "基于护盾", "基於護盾", "シールドに基づく" };
        translations["Rainbow Admin Color"] = { "Rainbow Admin Color", "彩虹管理员颜色", "彩虹管理員顏色", "レインボー管理者色" };
        translations["Rainbow Speed"] = { "Rainbow Speed", "彩虹速度", "彩虹速度", "レインボー速度" };
        translations["Outline Size"] = { "Outline Size", "轮廓大小", "輪廓大小", "輪郭サイズ" };
        translations["Highlight Scale"] = { "Highlight Scale", "高亮比例", "高亮比例", "ハイライトスケール" };
        translations["Knocked Color"] = { "Knocked Color", "击倒颜色", "擊倒顏色", "ダウン色" };
        translations["Admin Color"] = { "Admin Color", "管理员颜色", "管理員顏色", "管理者色" };
        translations["Enable Item Glow"] = { "Enable Item Glow", "启用物品发光", "啟用物品發光", "アイテムグローを有効化" };
        translations["Item Types"] = { "Item Types", "物品类型", "物品類型", "アイテムタイプ" };
        translations["Ammo"] = { "Ammo", "弹药", "彈藥", "弾薬" };
        translations["Weapons"] = { "Weapons", "武器", "武器", "武器" };
        translations["Item Rarity"] = { "Item Rarity", "物品稀有度", "物品稀有度", "アイテムレア度" };
        translations["Grey"] = { "Grey", "灰色", "灰色", "グレー" };
        translations["Blue"] = { "Blue", "蓝色", "藍色", "ブルー" };
        translations["Purple"] = { "Purple", "紫色", "紫色", "パープル" };
        translations["Gold"] = { "Gold", "金色", "金色", "ゴールド" };
        translations["Red"] = { "Red", "红色", "紅色", "レッド" };

        // TriggerBot相关
        translations["TRIGGERBOT SETTINGS"] = { "TRIGGERBOT SETTINGS", "扳机机器人设置", "扳機機器人設置", "トリガーボット設定" };
        translations["Basic"] = { "Basic", "基础", "基礎", "基本" };
        translations["Keys"] = { "Keys", "按键", "按鍵", "キー" };
        translations["Enable TriggerBot"] = { "Enable TriggerBot", "启用扳机机器人", "啟用扳機機器人", "トリガーボットを有効化" };
        translations["Auto Fire when Aimed At"] = { "Auto Fire when Aimed At", "被瞄准时自动开火", "被瞄準時自動開火", "狙われた時に自動発射" };
        translations["Calculate Bone Position"] = { "Calculate Bone Position", "计算骨骼位置", "計算骨骼位置", "骨位置を計算" };
        translations["Trigger Mode"] = { "Trigger Mode", "扳机模式", "扳機模式", "トリガーモード" };
        translations["Trigger Delay (ms)"] = { "Trigger Delay (ms)", "扳机延迟(毫秒)", "扳機延遲(毫秒)", "トリガー遅延(ms)" };
        translations["Show Hitboxes"] = { "Show Hitboxes", "显示命中框", "顯示命中框", "ヒットボックスを表示" };
        translations["Show Prediction"] = { "Show Prediction", "显示预测", "顯示預測", "予測を表示" };
        translations["Distance Scale"] = { "Distance Scale", "距离比例", "距離比例", "距離スケール" };
        translations["Minimum Radius"] = { "Minimum Radius", "最小半径", "最小半徑", "最小半径" };
        translations["Maximum Radius"] = { "Maximum Radius", "最大半径", "最大半徑", "最大半径" };
        translations["Note: Your TriggerType Settings is AimeAt only! So the Hitbox Settings will not use!"] = { "Note: Your TriggerType Settings is AimeAt only! So the Hitbox Settings will not use!", "注意：您的触发类型设置仅为被瞄准！因此不会使用命中框设置！", "注意：您的觸發類型設置僅為被瞄準！因此不會使用命中框設置！", "注意：トリガータイプ設定はエイムアットのみです！そのためヒットボックス設定は使用されません！" };
        translations["Hitbox Configuration"] = { "Hitbox Configuration", "命中框配置", "命中框配置", "ヒットボックス設定" };
        translations["Hitbox"] = { "Hitbox", "命中框", "命中框", "ヒットボックス" };
        translations["Enabled"] = { "Enabled", "启用", "啟用", "有効" };
        translations["Radius"] = { "Radius", "半径", "半徑", "半径" };
        translations["Color"] = { "Color", "颜色", "顏色", "色" };
        translations["Hip"] = { "Hip", "臀部", "臀部", "腰" };
        translations["Unknown"] = { "Unknown", "未知", "未知", "不明" };
        translations["Triggerbot Key Bindings"] = { "Triggerbot Key Bindings", "扳机机器人按键绑定", "扳機機器人按鍵綁定", "トリガーボットキーバインド" };
        translations["Standard Trigger"] = { "Standard Trigger", "标准扳机", "標準扳機", "標準トリガー" };
        translations["Activates trigger without aiming"] = { "Activates trigger without aiming", "无需瞄准即可激活扳机", "無需瞄準即可激活扳機", "エイム無しでトリガーを有効化" };
        translations["Magnetic Trigger"] = { "Magnetic Trigger", "磁性扳机", "磁性扳機", "磁気トリガー" };
        translations["Aims at target while triggering"] = { "Aims at target while triggering", "触发扳机的同时瞄准目标", "觸發扳機的同時瞄準目標", "トリガー中にターゲットを狙う" };
        translations["Configure what items to display and their colors. You can enable/disable entire categories or individual items."] = {
            "Configure what items to display and their colors. You can enable/disable entire categories or individual items.",
            "配置要显示的物品及其颜色。您可以启用/禁用整个类别或单个物品。",
            "配置要顯示的物品及其顏色。您可以啟用/禁用整個類別或單個物品。",
            "表示するアイテムとその色を設定します。カテゴリ全体または個々のアイテムを有効/無効にできます。"
        };

        translations["Use custom color"] = {
            "Use custom color",
            "使用自定义颜色",
            "使用自定義顏色",
            "カスタムカラーを使用"
        };
        translations["UseController"] = { "Use Controller", "使用手柄", "使用手柄", "コントローラーを使用" };
        translations["None"] = { "None", "无", "無", "なし" };
        translations["Square Button"] = { "Square Button", "方块按钮", "方塊按鈕", "四角ボタン" };
        translations["Triangle Button"] = { "Triangle Button", "三角按钮", "三角按鈕", "三角ボタン" };
        translations["Circle Button"] = { "Circle Button", "圆圈按钮", "圓圈按鈕", "丸ボタン" };
        translations["Cross Button"] = { "Cross Button", "十字按钮", "十字按鈕", "バツボタン" };
        translations["Left Shoulder"] = { "Left Shoulder", "左肩", "左肩", "L1ボタン" };
        translations["Right Shoulder"] = { "Right Shoulder", "右肩", "右肩", "R1ボタン" };
        translations["Map Button"] = { "Map Button", "地图按钮", "地圖按鈕", "マップボタン" };
        translations["Settings Button"] = { "Settings Button", "设置按钮", "設置按鈕", "設定ボタン" };
        translations["Left Trigger"] = { "Left Trigger", "左扳机", "左扳機", "L2トリガー" };
        translations["Right Trigger"] = { "Right Trigger", "右扳机", "右扳機", "R2トリガー" };
        translations["D-Pad Up"] = { "D-Pad Up", "十字键上", "十字鍵上", "方向キー上" };
        translations["D-Pad Left"] = { "D-Pad Left", "十字键左", "十字鍵左", "方向キー左" };
        translations["D-Pad Right"] = { "D-Pad Right", "十字键右", "十字鍵右", "方向キー右" };
        translations["D-Pad Down"] = { "D-Pad Down", "十字键下", "十字鍵下", "方向キー下" };

        // ESP新增动画相关
        translations["Animation"] = { "Animation", "动画", "動畫", "アニメーション" };
        translations["Animation Settings"] = { "Animation Settings", "动画设置", "動畫設置", "アニメーション設定" };
        translations["Enable Animation"] = { "Enable Animation", "启用动画", "啟用動畫", "アニメーションを有効化" };
        translations["When enabled, ESP elements fade out when close to screen center"] = {
            "When enabled, ESP elements fade out when close to screen center",
            "启用后，ESP元素在靠近屏幕中心时会淡出",
            "啟用後，ESP元素在靠近屏幕中心時會淡出",
            "有効にすると、ESP要素は画面中央に近づくとフェードアウトします"
        };
        translations["This helps prevent blocking your view during combat"] = {
            "This helps prevent blocking your view during combat",
            "这有助于防止在战斗中阻碍您的视线",
            "這有助於防止在戰鬥中阻礙您的視線",
            "これは戦闘中に視界を遮るのを防ぐのに役立ちます"
        };
        translations["Min Distance"] = { "Min Distance", "最小距离", "最小距離", "最小距離" };
        translations["ESP elements closer than this to screen center become completely transparent"] = {
            "ESP elements closer than this to screen center become completely transparent",
            "比这更靠近屏幕中心的ESP元素将完全透明",
            "比這更靠近屏幕中心的ESP元素將完全透明",
            "画面中央からこれより近いESP要素は完全に透明になります"
        };
        translations["Max Distance"] = { "Max Distance", "最大距离", "最大距離", "最大距離" };
        translations["ESP elements further than this from screen center are fully visible"] = {
            "ESP elements further than this from screen center are fully visible",
            "距离屏幕中心超过这个距离的ESP元素完全可见",
            "距離屏幕中心超過這個距離的ESP元素完全可見",
            "画面中央からこれより遠いESP要素は完全に表示されます"
        };
        translations["Fade Scale"] = { "Fade Scale", "淡出比例", "淡出比例", "フェードスケール" };
        translations["Adjusts how quickly ESP fades in based on distance"] = {
            "Adjusts how quickly ESP fades in based on distance",
            "调整ESP基于距离淡入的速度",
            "調整ESP基於距離淡入的速度",
            "距離に基づいてESPがフェードインする速さを調整します"
        };
        translations["Higher values = faster fade in"] = {
            "Higher values = faster fade in",
            "值越高 = 淡入越快",
            "值越高 = 淡入越快",
            "値が高いほど = フェードインが速くなります"
        };

        // Aimbot新增人性化相关
        translations["Humanize Mode"] = { "Humanize Mode", "人性化模式", "人性化模式", "ヒューマナイズモード" };
        translations["Curve Offset"] = { "Curve Offset (Mode 1)", "曲线偏移 (模式1)", "曲線偏移 (模式1)", "カーブオフセット (モード1)" };
        translations["Bezier Path"] = { "Bezier Path (Mode 2)", "贝塞尔路径 (模式2)", "貝塞爾路徑 (模式2)", "ベジェパス (モード2)" };
        translations["Curve Offset Range"] = { "Curve Offset Range", "曲线偏移范围", "曲線偏移範圍", "カーブオフセット範囲" };
        translations["Offset Scale Factor"] = { "Offset Scale Factor", "偏移比例因子", "偏移比例因子", "オフセットスケール係数" };
        translations["Max Offset Ratio"] = { "Max Offset Ratio", "最大偏移比例", "最大偏移比例", "最大オフセット比率" };
        translations["Use Alternative Method"] = { "Use Alternative Method", "使用替代方法", "使用替代方法", "代替手法を使用" };
        translations["Alternative method provides more intuitive control over aiming offset"] = {
            "Alternative method provides more intuitive control over aiming offset",
            "替代方法提供更直观的瞄准偏移控制",
            "替代方法提供更直觀的瞄準偏移控制",
            "代替手法はエイムオフセットをより直感的に制御できます"
        };
        translations["Higher values = more accuracy (less offset)"] = {
            "Higher values = more accuracy (less offset)",
            "值越高 = 更精确 (偏移更小)",
            "值越高 = 更精確 (偏移更小)",
            "値が高いほど = 精度が高い (オフセットが小さい)"
        };
        translations["Normal range: 5-30 for alternative method"] = {
            "Normal range: 5-30 for alternative method",
            "正常范围：替代方法为5-30",
            "正常範圍：替代方法為5-30",
            "通常範囲: 代替手法の場合は5〜30"
        };
        translations["Normal range: 10-25 for original method"] = {
            "Normal range: 10-25 for original method",
            "正常范围：原始方法为10-25",
            "正常範圍：原始方法為10-25",
            "通常範囲: 元の手法の場合は10〜25"
        };
        translations["Controls the overall magnitude of offset"] = {
            "Controls the overall magnitude of offset",
            "控制偏移的整体幅度",
            "控制偏移的整體幅度",
            "オフセットの全体的な大きさを制御します"
        };
        translations["Lower values = smaller offset"] = {
            "Lower values = smaller offset",
            "值越低 = 偏移越小",
            "值越低 = 偏移越小",
            "値が低いほど = オフセットが小さい"
        };
        translations["Maximum offset as a percentage of target distance"] = {
            "Maximum offset as a percentage of target distance",
            "最大偏移占目标距离的百分比",
            "最大偏移佔目標距離的百分比",
            "ターゲット距離の割合としての最大オフセット"
        };
        translations["0.03 = up to 3% of the distance"] = {
            "0.03 = up to 3% of the distance",
            "0.03 = 距离的最多3%",
            "0.03 = 距離的最多3%",
            "0.03 = 距離の最大3%"
        };

        // UI控制按键相关翻译
        translations["Menu Key"] = { "Menu Key", "菜单键", "選單鍵", "メニューキー" };
        translations["End Key"] = { "End Key", "结束键", "結束鍵", "終了キー" };

        // KMBOX相关翻译
        translations["KMBOX SETTINGS"] = { "KMBOX SETTINGS", "键鼠适配器设置", "鍵鼠適配器設置", "キーボードマウスアダプタ設定" };
        translations["DeviceType"] = { "Device Type", "设备类型", "設備類型", "デバイスタイプ" };
        translations["BPro/Arduino/Makcu"] = { "BPro/Arduino/Makcu", "BPro/Arduino/Makcu", "BPro/Arduino/Makcu", "BPro/Arduino/Makcu" };
        translations["ComPort"] = { "COM Port", "串行端口", "串行端口", "COMポート" };
        translations["BaudRate"] = { "Baud Rate", "波特率", "波特率", "ボーレート" };
        translations["KmboxIP"] = { "Kmbox IP", "Kmbox IP地址", "Kmbox IP地址", "Kmbox IPアドレス" };
        translations["KmboxPort"] = { "Kmbox Port", "Kmbox 端口", "Kmbox 端口", "Kmbox ポート" };
        translations["KmboxUUID"] = { "Kmbox UUID", "Kmbox UUID", "Kmbox UUID", "Kmbox UUID" };
        translations["RememberKmboxInfo"] = { "Remember Kmbox Info", "记住Kmbox信息", "記住Kmbox信息", "Kmbox情報を記憶" };
        translations["Initialize"] = { "Initialize", "初始化", "初始化", "初期化" };
        translations["Success"] = { "Success", "成功", "成功", "成功" };
        translations["Failed or Timeout"] = { "Failed or Timeout", "失败或超时", "失敗或超時", "失敗またはタイムアウト" };

        translations["Spectator"] = { "Spectator", "观战者", "觀戰者", "観戦者" };
        translations["Show Spectator"] = { "Show Spectator", "显示观战者", "顯示觀戰者", "観戦者を表示" };
        translations["Total Spectators:"] = { "Total Spectators: ", "观战者总数: ", "觀戰者總數: ", "観戦者数: " };

        translations["Game Radar"] = { "Game Radar", "游戏雷达", "遊戲雷達", "ゲームレーダー" };
        translations["Map"] = { "Map", "地图", "地圖", "マップ" };
        translations["Players"] = { "Players", "玩家人数", "玩家人數", "プレイヤー数" };
        translations["Player"] = { "Player", "玩家", "玩家", "プレイヤー" };
        translations["Bot"] = { "Bot", "机器人", "機器人", "ボット" };
        translations["Mouse Coordinates"] = { "Mouse Coordinates", "鼠标坐标", "滑鼠坐標", "マウス座標" };
        translations["Center View"] = { "Center View", "视角跟随", "視角跟隨", "視点追従" };
        translations["Show Health"] = { "Show Health", "显示血条", "顯示血條", "体力表示" };
        translations["Show Name"] = { "Show Name", "显示名字", "顯示名字", "名前表示" };
        translations["Show Rank"] = { "Show Rank", "显示段位", "顯示段位", "ランク表示" };
        translations["Show Kills & Damage"] = { "Show Kills & Damage", "显示击杀伤害", "顯示擊殺傷害", "キル・ダメージ表示" };

        // WebRadar服务器日志
        translations["Starting web radar server..."] = { "Starting web radar server...", "正在启动网页雷达服务器...", "正在啟動網頁雷達服務器...", "ウェブレーダーサーバーを起動中..." };
        translations["Server started successfully. Please visit http://localhost:8080"] =
        { "Server started successfully. Please visit http://localhost:8080",
          "服务器启动成功。请访问 http://localhost:8080",
          "服務器啟動成功。請訪問 http://localhost:8080",
          "サーバーが正常に起動しました。http://localhost:8080 にアクセスしてください" };
        translations["Failed to start radar server"] = { "Failed to start radar server", "雷达服务器启动失败", "雷達服務器啟動失敗", "レーダーサーバーの起動に失敗しました" };
        translations["Server stopped"] = { "Server stopped", "服务器已停止", "服務器已停止", "サーバーが停止しました" };
        translations["File not found"] = { "File not found", "文件未找到", "文件未找到", "ファイルが見つかりません" };
        translations["No valid map data"] = { "No valid map data", "没有有效的地图数据", "沒有有效的地圖數據", "有効なマップデータがありません" };
        translations["Language changed to"] = { "Language changed to", "语言已更改为", "語言已更改為", "言語が変更されました" };

        // 地图相关
        translations["Unknown"] = { "Unknown", "未知", "未知", "不明" };

        translations["SuperGlide"] = { "Super Glide", "超级滑翔", "超級滑翔", "スーパーグライド"  };
        translations["TapStrafe"] = { "Tap Strafe", "自动TS", "自動TS", "タップストレイフ" };
        translations["BunnyHop"] = { "Bunny Hop", "连跳", "連跳", "バニーホップ" };
        translations["AutoGrapple"] = { "Auto Grapple", "超级钩爪", "超級鉤爪", "オートグラップル" };

        translations["Bounding Box Configuration"] = { "Bounding Box Configuration", "边界框配置", "邊界框配置", "境界ボックス設定" };
        translations["Line Thickness"] = { "Line Thickness", "线条粗细", "線條粗細", "線の太さ" };
        translations["Hitbox Expansion"] = { "Hitbox Expansion", "边界框扩展", "邊界框擴展", "ヒットボックス拡張" };
        translations["Close Range Threshold"] = { "Close Range Threshold", "近距离阈值", "近距離閾值", "近距離しきい値" };
        translations["Expands hitbox for easier triggering"] = { "Expands hitbox for easier triggering", "扩展边界框大小，使触发更容易", "擴展邊界框大小，使觸發更容易", "トリガーを容易にするためのヒットボックス拡張" };
        translations["Uses 3D bounding boxes for more accurate detection"] = { "Uses 3D bounding boxes for more accurate detection", "使用3D边界框判定更准确地检测准星位置", "使用3D邊界框判定更準確地檢測準星位置", "より正確な検出のために3D境界ボックスを使用" };
        translations["Automatically expands at close range"] = { "Automatically expands at close range", "近距离时自动扩大边界框并启用额外判定", "近距離時自動擴大邊界框並啟用額外判定", "近距離で自動的に拡大" };
        // 在Translation::initTranslations()中添加
        translations["Prediction System"] = { "Prediction System", "预测系统", "預測系統", "予測システム" };
        translations["AimCorrection Method"] = { "AimCorrection Method", "弹道修正法", "彈道修正法", "弾道修正法" };
        translations["Advanced Cache System"] = { "Advanced Cache System", "高级缓存系统", "高級緩存系統", "高度キャッシュシステム" };
        translations["Cache Timeout (ms)"] = { "Cache Timeout (ms)", "缓存超时(毫秒)", "緩存超時(毫秒)", "キャッシュタイムアウト(ms)" };
        translations["Prediction System Settings"] = { "Prediction System Settings", "预测系统设置", "預測系統設置", "予測システム設定" };
        translations["Basic Method: Simple and fast"] = { "Basic Method: Simple and fast", "基础方法：简单且快速", "基礎方法：簡單且快速", "基本的な方法：シンプルで高速" };
        translations["Advanced Method: More accurate but complex"] = { "Advanced Method: More accurate but complex", "高级方法：更准确但复杂", "高級方法：更準確但複雜", "高度な方法：より正確ですが複雑" };
        translations["Time before recalculating prediction (lower = more CPU usage)"] = { "Time before recalculating prediction (lower = more CPU usage)", "重新计算预测的时间间隔(越低=CPU使用率越高)", "重新計算預測的時間間隔(越低=CPU使用率越高)", "予測を再計算する時間（低いほどCPU使用率が高い）" };
        translations["OpenWebRadar"] = { "OpenWebRadar", "打开网页雷达", "打開網頁雷達", "ウェブレーダーを開く" };
        translations["Prediction"] = { "Prediction", "预测", "預測", "予測" };
        translations["General Settings"] = { "General Settings", "通用设置", "通用設置", "一般設定" };
        translations["Shield Check"] = { "Shield Check", "护盾检查", "護盾檢查", "シールドチェック" };
        translations["Skeleton Visible Color"] = { "Skeleton Visible Color", "骨架可见颜色", "骨架可見顏色", "スケルトン可視色" };
        translations["Skeleton Invisible Color"] = { "Skeleton InVisible Color", "骨架不可见颜色", "骨架不可見顏色", "スケルトン不可視色" };
        translations["ESP Thickness"] = { "ESP Thickness", "透视线条粗细", "透視線條粗細", "ESP線の太さ" };
        translations["Skeleton Thickness"] = { "Skeleton Thickness", "骨架线条粗细", "骨架線條粗細", "スケルトン線の太さ" };
        translations["Seer"] = { "Seer", "预言者", "預言者", "シーア" };
        translations["General"] = { "General", "通用", "通用", "一般" };
        translations["Active Color"] = { "Active Color", "激活颜色", "激活顏色", "アクティブ色" };
        translations["Advanced Options"] = { "Advanced Options", "高级选项", "高級選項", "詳細オプション" };
        translations["Show Debug Info"] = { "Show Debug Info", "显示调试信息", "顯示調試信息", "デバッグ情報を表示" };
        translations["Fusion Key"] = { "Fusion Key", "融合键", "融合鍵", "フュージョンキー" };
        translations["Radar Key"] = { "Radar key", "雷达键", "雷達鍵", "レーダーキー" };
        translations["SkyNade Settings"] = { "SkyNade Settings", "SkyNade设置", "SkyNade設置", "SkyNade設定" };
        translations["Enable SkyNade"] = { "Enable SkyNade", "启用SkyNade", "啟用SkyNade", "SkyNadeを有効化" };
        translations["SkyNade Key"] = { "SkyNade Key", "SkyNade按键", "SkyNade按鍵", "SkyNadeキー" };
        translations["Instructions: Hold a grenade and press the SkyNade key while targeting an enemy to automatically calculate the best angle."] = {
            "Instructions: Hold a grenade and press the SkyNade key while targeting an enemy to automatically calculate the best angle.",
            "使用说明：拿着手雷并按下SkyNade键同时瞄准敌人，自动计算最佳投掷角度。",
            "使用說明：拿著手雷並按下SkyNade鍵同時瞄準敵人，自動計算最佳投擲角度。",
            "使用方法：グレネードを持ちながらSkyNadeキーを押し、敵を狙うと最適な角度が自動計算されます。"
        };
        translations["Supported grenades: Thermite, Frag, Arc Star"] = {
            "Supported grenades: Thermite, Frag, Arc Star",
            "支持的手雷：热能手雷、破片手雷、电弧星",
            "支持的手雷：熱能手雷、破片手雷、電弧星",
            "対応グレネード：サーマイト、フラグ、アークスター"
        };
        translations["Visual Settings"] = { "Visual Settings", "视觉设置", "視覺設置", "視覚設定" };

        // Weapon name translations
        translations["None"] = { "None", "无", "無", "なし" };

        // Assault Rifles
        translations["R-301"] = { "R-301", "R-301", "R-301", "R-301" };
        translations["Hemlok"] = { "Hemlok", "平行步枪", "平行步槍", "ヘムロック" };
        translations["Hemlok (Crate)"] = { "Hemlok (Crate)", "平行步枪（空投）", "平行步槍（空投）", "ヘムロック（クレート）" };
        translations["Flatline"] = { "Flatline", "平行机枪", "平行機槍", "フラットライン" };
        translations["G7 Scout"] = { "G7 Scout", "G7侦查枪", "G7偵查槍", "G7スカウト" };
        translations["Havoc"] = { "Havoc", "哈沃克", "哈沃克", "ハボック" };
        translations["Havoc (Crate)"] = { "Havoc (Crate)", "哈沃克（空投）", "哈沃克（空投）", "ハボック（クレート）" };
        translations["Nemesis"] = { "Nemesis", "复仇女神", "復仇女神", "ネメシス" };
        translations["30-30 Repeater"] = { "30-30 Repeater", "30-30", "30-30", "30-30リピーター" };

        // LMGs
        translations["Spitfire"] = { "Spitfire", "喷火轻机枪", "噴火輕機槍", "スピットファイア" };
        translations["Devotion"] = { "Devotion", "专注轻机枪", "專注輕機槍", "ディボーション" };
        translations["Devotion (Crate)"] = { "Devotion (Crate)", "专注轻机枪（空投）", "專注輕機槍（空投）", "ディボーション（クレート）" };
        translations["L-STAR"] = { "L-STAR", "L-STAR", "L-STAR", "L-STAR" };
        translations["L-STAR (Crate)"] = { "L-STAR (Crate)", "L-STAR（空投）", "L-STAR（空投）", "L-STAR（クレート）" };
        translations["Rampage"] = { "Rampage", "暴走轻机枪", "暴走輕機槍", "ランページ" };

        // SMGs
        translations["Alternator"] = { "Alternator", "转换者", "轉換者", "オルタネーター" };
        translations["R-99"] = { "R-99", "R-99", "R-99", "R-99" };
        translations["R-99 (Crate)"] = { "R-99 (Crate)", "R-99（空投）", "R-99（空投）", "R-99（クレート）" };
        translations["Prowler"] = { "Prowler", "猎兽", "獵獸", "プラウラー" };
        translations["Prowler (Crate)"] = { "Prowler (Crate)", "猎兽（空投）", "獵獸（空投）", "プラウラー（クレート）" };
        translations["Volt"] = { "Volt", "电能冲锋枪", "電能衝鋒槍", "ボルト" };
        translations["C.A.R."] = { "C.A.R.", "CAR冲锋枪", "CAR衝鋒槍", "C.A.R." };

        // Shotguns
        translations["EVA-8"] = { "EVA-8", "EVA-8自动霰弹枪", "EVA-8自動霰彈槍", "EVA-8オート" };
        translations["EVA-8 (Crate)"] = { "EVA-8 (Crate)", "EVA-8自动霰弹枪（空投）", "EVA-8自動霰彈槍（空投）", "EVA-8オート（クレート）" };
        translations["Mastiff"] = { "Mastiff", "敖犬", "敖犬", "マスティフ" };
        translations["Mastiff (Crate)"] = { "Mastiff (Crate)", "敖犬（空投）", "敖犬（空投）", "マスティフ（クレート）" };
        translations["Peacekeeper"] = { "Peacekeeper", "和平捍卫者", "和平捍衛者", "ピースキーパー" };
        translations["Peacekeeper (Crate)"] = { "Peacekeeper (Crate)", "和平捍卫者（空投）", "和平捍衛者（空投）", "ピースキーパー（クレート）" };

        // Sniper/Marksman
        translations["Sentinel"] = { "Sentinel", "哨兵", "哨兵", "センチネル" };
        translations["Bocek Bow"] = { "Bocek Bow", "波塞克复合弓", "波塞克複合弓", "ボセック ボウ" };
        translations["Kraber"] = { "Kraber", "克莱伯", "克萊伯", "クレーバー" };
        translations["Charge Rifle"] = { "Charge Rifle", "充能步枪", "充能步槍", "チャージライフル" };
        translations["Longbow DMR"] = { "Longbow DMR", "长弓DMR", "長弓DMR", "ロングボウDMR" };
        translations["Triple Take"] = { "Triple Take", "三重式狙击枪", "三重式狙擊槍", "トリプルテイク" };
        translations["EPG"] = { "EPG", "能源榴弹枪", "能源榴彈槍", "EPG" };
        translations["EPG (Tethered)"] = { "EPG (Tethered)", "能源榴弹枪（锁定）", "能源榴彈槍（鎖定）", "EPG（テザー）" };

        // Pistols
        translations["P2020"] = { "P2020", "P2020", "P2020", "P2020" };
        translations["RE-45"] = { "RE-45", "RE-45自动手枪", "RE-45自動手槍", "RE-45オート" };
        translations["RE-45 (Crate)"] = { "RE-45 (Crate)", "RE-45自动手枪（空投）", "RE-45自動手槍（空投）", "RE-45オート（クレート）" };
        translations["Wingman"] = { "Wingman", "辅助手枪", "輔助手槍", "ウィングマン" };
        translations["Wingman (Crate)"] = { "Wingman (Crate)", "辅助手枪（空投）", "輔助手槍（空投）", "ウィングマン（クレート）" };
        translations["Mozambique"] = { "Mozambique", "莫桑比克", "莫桑比克", "モザンビーク" };

        // Mozambique variants
        translations["Mozambique (Light)"] = { "Mozambique (Light)", "莫桑比克（轻型弹）", "莫桑比克（輕型彈）", "モザンビーク（ライト）" };
        translations["Mozambique (Energy)"] = { "Mozambique (Energy)", "莫桑比克（能量弹）", "莫桑比克（能量彈）", "モザンビーク（エネルギー）" };
        translations["Mozambique (Sniper)"] = { "Mozambique (Sniper)", "莫桑比克（狙击弹）", "莫桑比克（狙擊彈）", "モザンビーク（スナイパー）" };
        translations["Mozambique (Heavy)"] = { "Mozambique (Heavy)", "莫桑比克（重型弹）", "莫桑比克（重型彈）", "モザンビーク（ヘビー）" };
        translations["Mozambique (EPG)"] = { "Mozambique (EPG)", "莫桑比克（EPG）", "莫桑比克（EPG）", "モザンビーク（EPG）" };

        // Throwables
        translations["Frag Grenade"] = { "Frag Grenade", "破片手雷", "破片手雷", "フラググレネード" };
        translations["Arc Star"] = { "Arc Star", "电弧星", "電弧星", "アークスター" };
        translations["Thermite Grenade"] = { "Thermite Grenade", "铝热剂手雷", "鋁熱劑手雷", "サーマイトグレネード" };
        translations["Silence"] = { "Silence", "亡灵沉默手雷", "亡靈沉默手雷", "サイレンス" };
        translations["Throwing Knife"] = { "Throwing Knife", "投掷刀", "投擲刀", "投げナイフ" };

        // Special Weapons
        translations["Titan Sword"] = { "Titan Sword", "泰坦剑", "泰坦劍", "タイタンソード" };
        translations["Mounted Turret"] = { "Mounted Turret", "安装炮塔", "安裝炮塔", "設置タレット" };
        translations["Placeable Turret"] = { "Placeable Turret", "可放置炮塔", "可放置炮塔", "設置可能タレット" };
        translations["Power Sword"] = { "Power Sword", "能量剑", "能量劍", "パワーソード" };

        translations["Empty Handed"] = { "Empty Handed", "空手", "空手", "素手" };
        translations["SkyNade helps you automatically calculate the optimal projection angle for grenades."] = {
    "SkyNade helps you automatically calculate the optimal projection angle for grenades.",
    "SkyNade帮助您自动计算手雷的最佳投掷角度。",
    "SkyNade幫助您自動計算手榴彈的最佳投擲角度。",
    "SkyNadeは手榴弾の最適な投射角度を自動的に計算します。"
        };

        translations["Config Switcher"] = { "Config Switcher", "配置切换器", "配置切換器", "設定スイッチャー" };
        translations["Enable Switcher"] = { "Enable Switcher", "启用切换器", "啟用切換器", "スイッチャーを有効化" };
        translations["Previous Config"] = { "Previous Config", "上一个配置", "上一個配置", "前の設定" };
        translations["Next Config"] = { "Next Config", "下一个配置", "下一個配置", "次の設定" };
        translations["Config List"] = { "Config List", "配置列表", "配置列表", "設定リスト" };
        translations["Add Config"] = { "Add Config", "添加配置", "添加配置", "設定を追加" };
        translations["Select a Config to add:"] = { "Select a Config to add:", "选择要添加的配置:", "選擇要添加的配置:", "追加する設定を選択:" };
        translations["Phoenix Kit"] = { "Phoenix Kit", "凤凰治疗包", "鳳凰治療包", "フェニックスキット" };
        translations["Med-Kit"] = { "Med-Kit", "医疗包", "醫療包", "メドキット" };
        translations["Syringe"] = { "Syringe", "注射器", "注射器", "注射器" };
        translations["Shield-Cell"] = { "Shield-Cell", "护盾电池", "護盾電池", "シールドセル" };
        translations["Shield-Battery"] = { "Shield-Battery", "护盾电池组", "護盾電池組", "シールドバッテリー" };
        translations["P2020"] = { "P2020", "P2020", "P2020", "P2020" };
        translations["RE-45"] = { "RE-45", "RE-45自动手枪", "RE-45自動手槍", "RE-45オート" };
        translations["Alternator"] = { "Alternator", "转换者", "轉換者", "オルタネーター" };
        translations["R-301"] = { "R-301", "R-301", "R-301", "R-301" };
        translations["M600 Spit-fire"] = { "M600 Spit-fire", "喷火轻机枪", "噴火輕機槍", "スピットファイア" };
        translations["G7 Scout"] = { "G7 Scout", "G7侦查枪", "G7偵查槍", "G7スカウト" };
        translations["Light Ammo"] = { "Light Ammo", "轻型弹药", "輕型彈藥", "ライトアモ" };
        translations["R-99"] = { "R-99", "R-99", "R-99", "R-99" };
        translations["Devotion LMG"] = { "Devotion LMG", "专注轻机枪", "專注輕機槍", "ディボーション" };
        translations["L-STAR"] = { "L-STAR", "L-STAR", "L-STAR", "L-STAR" };
        translations["Triple Take"] = { "Triple Take", "三重式狙击枪", "三重式狙擊槍", "トリプルテイク" };
        translations["Nemesis Burst AR"] = { "Nemesis Burst AR", "复仇女神", "復仇女神", "ネメシス" };
        translations["Energy Ammo"] = { "Energy Ammo", "能量弹药", "能量彈藥", "エネルギーアモ" };
        translations["VOLT"] = { "VOLT", "电能冲锋枪", "電能衝鋒槍", "ボルト" };
        translations["VK47 FlatLine"] = { "VK47 FlatLine", "平行机枪", "平行機槍", "フラットライン" };
        translations["Hemlock"] = { "Hemlock", "平行步枪", "平行步槍", "ヘムロック" };
        translations["Prowler"] = { "Prowler", "猎兽", "獵獸", "プラウラー" };
        translations["30-30 Repeater"] = { "30-30 Repeater", "30-30", "30-30", "30-30リピーター" };
        translations["Rampage"] = { "Rampage", "暴走轻机枪", "暴走輕機槍", "ランページ" };
        translations["Heavy Ammo"] = { "Heavy Ammo", "重型弹药", "重型彈藥", "ヘビーアモ" };
        translations["C.A.R"] = { "C.A.R", "CAR冲锋枪", "CAR衝鋒槍", "C.A.R." };
        translations["Wing Man"] = { "Wing Man", "辅助手枪", "輔助手槍", "ウィングマン" };
        translations["Long Bow"] = { "Long Bow", "长弓DMR", "長弓DMR", "ロングボウDMR" };
        translations["Charge Rifle"] = { "Charge Rifle", "充能步枪", "充能步槍", "チャージライフル" };
        translations["Sentinel"] = { "Sentinel", "哨兵", "哨兵", "センチネル" };
        translations["Sniper Ammo"] = { "Sniper Ammo", "狙击弹药", "狙擊彈藥", "スナイパーアモ" };
        translations["Mozambique"] = { "Mozambique", "莫桑比克", "莫桑比克", "モザンビーク" };
        translations["EVA"] = { "EVA", "EVA-8自动霰弹枪", "EVA-8自動霰彈槍", "EVA-8オート" };
        translations["Shotgun Ammo"] = { "Shotgun Ammo", "霰弹弹药", "霰彈彈藥", "ショットガンアモ" };
        translations["Mastiff"] = { "Mastiff", "敖犬", "敖犬", "マスティフ" };
        translations["Thermite"] = { "Thermite", "铝热剂手雷", "鋁熱劑手雷", "サーマイトグレネード" };
        translations["Frag Grenade"] = { "Frag Grenade", "破片手雷", "破片手雷", "フラググレネード" };
        translations["Arc_star"] = { "Arc_star", "电弧星", "電弧星", "アークスター" };
        translations["Bow"] = { "Bow", "波塞克复合弓", "波塞克複合弓", "ボセック ボウ" };
        translations["Kraber"] = { "Kraber", "克莱伯", "克萊伯", "クレーバー" };
        translations["Havoc"] = { "Havoc", "哈沃克", "哈沃克", "ハボック" };
        translations["PeaceKeeper"] = { "PeaceKeeper", "和平捍卫者", "和平捍衛者", "ピースキーパー" };
        // 头盔
        translations["Helmet5"] = { "Helmet5", "5级头盔", "5級頭盔", "レベル5ヘルメット" };
        translations["Helmet4"] = { "Helmet4", "4级头盔", "4級頭盔", "レベル4ヘルメット" };
        translations["EVO Cache"] = { "EVO Cache", "升级缓存", "升級緩存", "EVOキャッシュ" };
        translations["Backpack4"] = { "Backpack4", "4级背包", "4級背包", "レベル4バックパック" };
        translations["Backpack3"] = { "Backpack3", "3级背包", "3級背包", "レベル3バックパック" };
        translations["Backpack2"] = { "Backpack2", "2级背包", "2級背包", "レベル2バックパック" };
        translations["Backpack"] = { "Backpack", "1级背包", "1級背包", "レベル1バックパック" };

        // 护盾
        translations["Knockdown Shield3"] = { "Knockdown Shield3", "3级击倒护盾", "3級擊倒護盾", "レベル3ノックダウンシールド" };
        translations["Knockdown Shield2"] = { "Knockdown Shield2", "2级击倒护盾", "2級擊倒護盾", "レベル2ノックダウンシールド" };
        translations["Knockdown Shield"] = { "Knockdown Shield", "1级击倒护盾", "1級擊倒護盾", "レベル1ノックダウンシールド" };
        translations["Shield3"] = { "Shield3", "3级护甲", "3級護甲", "レベル3ボディシールド" };
        translations["Shield2"] = { "Shield2", "2级护甲", "2級護甲", "レベル2ボディシールド" };
        translations["Shield"] = { "Shield", "1级护甲", "1級護甲", "レベル1ボディシールド" };

        // 配件
        translations["Sniper Threat"] = { "Sniper Threat", "狙击威胁镜", "狙擊威脅鏡", "スナイパースレットサイト" };
        translations["Suppressor"] = { "Suppressor", "消音器", "消音器", "サプレッサー" };
        translations["Light Mag"] = { "Light Mag", "轻型弹匣", "輕型彈匣", "ライトマガジン" };
        translations["Heavy Mag"] = { "Heavy Mag", "重型弹匣", "重型彈匣", "ヘビーマガジン" };
        translations["Energy Mag"] = { "Energy Mag", "能量弹匣", "能量彈匣", "エネルギーマガジン" };
        translations["Sniper Mag"] = { "Sniper Mag", "狙击弹匣", "狙擊彈匣", "スナイパーマガジン" };
        translations["Shotgun Bolt"] = { "Shotgun Bolt", "霰弹枪栓", "霰彈槍栓", "ショットガンボルト" };
        translations["Boosted_Loader"] = { "Boosted_Loader", "强化装填器", "強化裝填器", "ブーステッドローダー" };
        translations["GunShield"] = { "GunShield", "枪盾", "槍盾", "ガンシールド" };
        translations["SelectFire"] = { "SelectFire", "选择射击", "選擇射擊", "セレクトファイア" };

        // 瞄准镜
        translations["Hcog-3x"] = { "Hcog-3x", "3倍全息", "3倍全息", "3倍HCOG" };
        translations["Holo2x-4x"] = { "Holo2x-4x", "2-4倍可变全息", "2-4倍可變全息", "2-4倍可変ホロ" };
        translations["sniper4x8x"] = { "sniper4x8x", "4-8倍可变狙击镜", "4-8倍可變狙擊鏡", "4-8倍可変スナイパー" };
        translations["Hcog-2x"] = { "Hcog-2x", "2倍全息", "2倍全息", "2倍HCOG" };
        translations["Holo1x-2x"] = { "Holo1x-2x", "1-2倍可变全息", "1-2倍可變全息", "1-2倍可変ホロ" };
        translations["Hcog-1x"] = { "Hcog-1x", "1倍全息", "1倍全息", "1倍HCOG" };

        // 特殊物品
        translations["Laser sight"] = { "Laser sight", "激光瞄准器", "激光瞄準器", "レーザーサイト" };
        translations["Sniper6x"] = { "Sniper6x", "6倍狙击镜", "6倍狙擊鏡", "6倍スナイパー" };
        translations["Stock"] = { "Stock", "普通枪托", "普通槍托", "ストック" };
        translations["Sniper Stock"] = { "Sniper Stock", "狙击枪托", "狙擊槍托", "スナイパーストック" };
        translations["Evac Tower"] = { "Evac Tower", "撤离塔", "撤離塔", "エバックタワー" };
        translations["Accelerant"] = { "Accelerant", "终极加速剂", "終極加速劑", "アルティメットアクセラント" };

        translations["SkyNade"] = { "SkyNade", "高抛雷", "高抛雷", "スカイナーデ" };

        translations["Medical Items"] = { "Medical Items", "医疗物品", "醫療物品", "医療アイテム" };
        translations["Light Weapons"] = { "Light Weapons", "轻型武器", "輕型武器", "ライト武器" };
        translations["Shotguns"] = { "Shotguns", "霰弹枪", "霰彈槍", "ショットガン" };
        translations["Energy Weapons"] = { "Energy Weapons", "能量武器", "能量武器", "エネルギー武器" };
        translations["Heavy Weapons"] = { "Heavy Weapons", "重型武器", "重型武器", "ヘビー武器" };
        translations["Sniper Weapons"] = { "Sniper Weapons", "狙击武器", "狙擊武器", "スナイパー武器" };
        translations["Level 1 Attachments"] = { "Level 1 Attachments", "1级配件", "1級配件", "レベル1アタッチメント" };
        translations["Level 2 Attachments"] = { "Level 2 Attachments", "2级配件", "2級配件", "レベル2アタッチメント" };
        translations["Level 3 Attachments"] = { "Level 3 Attachments", "3级配件", "3級配件", "レベル3アタッチメント" };
        translations["Level 4 Attachments"] = { "Level 4 Attachments", "4级配件", "4級配件", "レベル4アタッチメント" };
        translations["Level 1 Armor"] = { "Level 1 Armor", "1级护甲", "1級護甲", "レベル1アーマー" };
        translations["Level 2 Armor"] = { "Level 2 Armor", "2级护甲", "2級護甲", "レベル2アーマー" };
        translations["Level 3 Armor"] = { "Level 3 Armor", "3级护甲", "3級護甲", "レベル3アーマー" };
        translations["Level 4 Armor"] = { "Level 4 Armor", "4级护甲", "4級護甲", "レベル4アーマー" };
        translations["Rare Weapons"] = { "Rare Weapons", "稀有武器", "稀有武器", "レア武器" };
        translations["Grenades"] = { "Grenades", "手雷", "手雷", "グレネード" };

        translations["Enable"] = { "Enable", "启用", "啟用", "有効化" };
        translations["Enabled"] = { "Enabled", "已启用", "已啟用", "有効" };
        translations["Enable SkyNade"] = { "Enable SkyNade", "启用高抛雷", "啟用高抛雷", "高抛きグレネードを有効化" };
        translations["Current Default Config:"] = { "Current Default Config:", "当前默认配置:", "當前默認配置:", "現在のデフォルト設定:" };
        translations["Delete Default"] = { "Delete Default", "删除默认", "刪除默認", "デフォルトを削除" };
}

public:
    // 初始化翻译系统
    static void init() {
        initTranslations();
    }

    // 获取翻译
    static const char* get(const std::string& key) {
        auto it = translations.find(key);
        if (it != translations.end() && currentLanguage < it->second.size()) {
            return it->second[currentLanguage].c_str();
        }

        // 找不到翻译，返回原始键
        return key.c_str();
    }

    // 设置当前语言
    static void setLanguage(Language lang) {
        if (lang >= 0 && lang < LanguageCount) {
            currentLanguage = lang;
        }
    }

    // 获取当前语言
    static Language getCurrentLanguage() {
        return currentLanguage;
    }

    // 获取语言名称
    static const char* getLanguageName(Language lang) {
        static const char* names[] = {
            "English",
            "简体中文",
            "繁體中文",
            "日本語"
        };

        if (lang >= 0 && lang < LanguageCount) {
            return names[lang];
        }
        return "Unknown";
    }

    // 获取当前语言名称
    static const char* getCurrentLanguageName() {
        return getLanguageName(currentLanguage);
    }
};

// 简单的翻译宏
#define t_(key) Translation::get(key)