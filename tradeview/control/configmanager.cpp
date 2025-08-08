#include "configmanager.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>

namespace btra::gui {

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
    , m_updateInterval(1000)
    , m_maxInstruments(20)
    , m_configLoaded(false)
    , m_theme("dark")
    , m_language("zh_CN")
    , m_autoRefresh(true)
    , m_refreshInterval(5000)
    , m_defaultTimeframe("1m")
    , m_maxCandlesticks(1000)
    , m_defaultZoomFactor(1.0)
    , m_dataSource("simulation")
{
}

ConfigManager::~ConfigManager()
{
}

bool ConfigManager::loadConfig(const QString& configPath)
{
    m_configPath = configPath;
    
    // 如果路径是相对路径，尝试从应用程序目录加载
    QString fullPath = configPath;
    if (!QDir::isAbsolutePath(configPath)) {
        // 尝试从应用程序目录加载
        QString appDir = QCoreApplication::applicationDirPath();
        fullPath = appDir + "/" + configPath;
        
        // 如果不存在，尝试从当前工作目录加载
        if (!QFile::exists(fullPath)) {
            fullPath = QDir::currentPath() + "/" + configPath;
        }
    }
    
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = QString("Cannot open main configuration file: %1").arg(fullPath);
        qWarning() << error;
        emit configLoadError(error);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString error = QString("JSON parse error: %1").arg(parseError.errorString());
        qWarning() << error;
        emit configLoadError(error);
        return false;
    }
    
    if (!parseGuiConfig(jsonDoc)) {
        QString error = "Failed to parse main configuration data";
        qWarning() << error;
        emit configLoadError(error);
        return false;
    }
    
    if (!validateGuiConfig()) {
        QString error = "Failed to validate main configuration data";
        qWarning() << error;
        emit configLoadError(error);
        return false;
    }

    if (!core_config_file_.isEmpty()) {
        core_engine_cfg_ = MainCfg(core_config_file_.toStdString());
    }
    
    // Load instruments configuration file
    if (!m_instrumentsPath.isEmpty()) {
        if (!loadInstrumentsConfig(m_instrumentsPath)) {
            qWarning() << "Cannot load instruments configuration file, using default settings";
        }
    }
    
    m_configLoaded = true;
    qInfo() << "Main configuration file loaded successfully:" << fullPath;
    
    emit configLoaded();
    return true;
}

QStringList ConfigManager::getEnabledInstruments() const
{
    QStringList enabled;
    for (const auto& instrument : m_instruments) {
        if (instrument.enabled) {
            enabled.append(instrument.symbol);
        }
    }
    return enabled;
}

QStringList ConfigManager::getDefaultInstruments() const
{
    return m_defaultInstruments;
}

QVariantList ConfigManager::getAllInstruments() const
{
    QVariantList result;
    for (const auto& instrument : m_instruments) {
        QVariantMap map;
        map["symbol"] = instrument.symbol;
        map["name"] = instrument.name;
        map["type"] = instrument.type;
        map["exchange"] = instrument.exchange;
        map["enabled"] = instrument.enabled;
        result.append(map);
    }
    return result;
}

QVariantMap ConfigManager::getInstrumentConfig(const QString& symbol) const
{
    for (const auto& instrument : m_instruments) {
        if (instrument.symbol == symbol) {
            QVariantMap map;
            map["symbol"] = instrument.symbol;
            map["name"] = instrument.name;
            map["type"] = instrument.type;
            map["exchange"] = instrument.exchange;
            map["enabled"] = instrument.enabled;
            return map;
        }
    }
    return QVariantMap();
}

bool ConfigManager::isInstrumentEnabled(const QString& symbol) const
{
    for (const auto& instrument : m_instruments) {
        if (instrument.symbol == symbol) {
            return instrument.enabled;
        }
    }
    return false;
}

int ConfigManager::getUpdateInterval() const
{
    return m_updateInterval;
}

int ConfigManager::getMaxInstruments() const
{
    return m_maxInstruments;
}

bool ConfigManager::loadInstrumentsConfig(const QString& instrumentsPath)
{
    m_instrumentsPath = instrumentsPath;
    
    // 如果路径是相对路径，尝试从应用程序目录加载
    QString fullPath = instrumentsPath;
    if (!QDir::isAbsolutePath(instrumentsPath)) {
        // 尝试从应用程序目录加载
        QString appDir = QCoreApplication::applicationDirPath();
        fullPath = appDir + "/" + instrumentsPath;
        
        // 如果不存在，尝试从当前工作目录加载
        if (!QFile::exists(fullPath)) {
            fullPath = QDir::currentPath() + "/" + instrumentsPath;
        }
    }
    
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = QString("Cannot open instruments configuration file: %1").arg(fullPath);
        qWarning() << error;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString error = QString("JSON parse error: %1").arg(parseError.errorString());
        qWarning() << error;
        return false;
    }
    
    if (!parseInstrumentsConfig(jsonDoc)) {
        QString error = "Failed to parse instruments configuration data";
        qWarning() << error;
        return false;
    }
    
    if (!validateInstrumentsConfig()) {
        QString error = "Failed to validate instruments configuration data";
        qWarning() << error;
        return false;
    }
    
    qInfo() << "Instruments configuration file loaded successfully:" << fullPath;
    qInfo() << "Loaded" << m_instruments.size() << "instruments";
    
    return true;
}

bool ConfigManager::reloadConfig()
{
    return loadConfig(m_configPath);
}

QString ConfigManager::getTheme() const
{
    return m_theme;
}

QString ConfigManager::getLanguage() const
{
    return m_language;
}

bool ConfigManager::getAutoRefresh() const
{
    return m_autoRefresh;
}

int ConfigManager::getRefreshInterval() const
{
    return m_refreshInterval;
}

QString ConfigManager::getDefaultTimeframe() const
{
    return m_defaultTimeframe;
}

int ConfigManager::getMaxCandlesticks() const
{
    return m_maxCandlesticks;
}

double ConfigManager::getDefaultZoomFactor() const
{
    return m_defaultZoomFactor;
}

QStringList ConfigManager::getDefaultSymbols() const
{
    return m_defaultSymbols;
}

QString ConfigManager::getDataSource() const
{
    return m_dataSource;
}

bool ConfigManager::parseGuiConfig(const QJsonDocument& jsonDoc)
{
    if (!jsonDoc.isObject()) {
        qWarning() << "Main configuration file root must be an object";
        return false;
    }
    
    QJsonObject root = jsonDoc.object();
    
    // Parse config section
    if (root.contains("config") && root["config"].isObject()) {
        QJsonObject configObj = root["config"].toObject();

        /* Parse core engine config file path */
        if (configObj.contains("core_engine_config") && configObj["core_engine_config"].isString()) {
            core_config_file_ = configObj["core_engine_config"].toString();
        }
        
        // Parse instruments_file path
        if (configObj.contains("instruments_file") && configObj["instruments_file"].isString()) {
            m_instrumentsPath = configObj["instruments_file"].toString();
        }

        if (configObj.contains("simulation") && configObj["simulation"].isBool()) {
            simulation_ = configObj["simulation"].toBool();
        }
        
        // Parse theme
        if (configObj.contains("theme") && configObj["theme"].isString()) {
            m_theme = configObj["theme"].toString();
        }
        
        // Parse language
        if (configObj.contains("language") && configObj["language"].isString()) {
            m_language = configObj["language"].toString();
        }
        
        // Parse auto refresh
        if (configObj.contains("auto_refresh") && configObj["auto_refresh"].isBool()) {
            m_autoRefresh = configObj["auto_refresh"].toBool();
        }
        
        // Parse refresh interval
        if (configObj.contains("refresh_interval") && configObj["refresh_interval"].isDouble()) {
            m_refreshInterval = configObj["refresh_interval"].toInt();
        }
    }
    
    // Parse UI section
    if (root.contains("ui") && root["ui"].isObject()) {
        QJsonObject uiObj = root["ui"].toObject();
        
        // Parse chart configuration
        if (uiObj.contains("chart") && uiObj["chart"].isObject()) {
            QJsonObject chartObj = uiObj["chart"].toObject();
            
            if (chartObj.contains("default_timeframe") && chartObj["default_timeframe"].isString()) {
                m_defaultTimeframe = chartObj["default_timeframe"].toString();
            }
            
            if (chartObj.contains("max_candlesticks") && chartObj["max_candlesticks"].isDouble()) {
                m_maxCandlesticks = chartObj["max_candlesticks"].toInt();
            }
            
            if (chartObj.contains("zoom_factor") && chartObj["zoom_factor"].isDouble()) {
                m_defaultZoomFactor = chartObj["zoom_factor"].toDouble();
            }
        }
    }
    
    // Parse data section
    if (root.contains("data") && root["data"].isObject()) {
        QJsonObject dataObj = root["data"].toObject();
        
        if (dataObj.contains("default_symbols") && dataObj["default_symbols"].isArray()) {
            QJsonArray symbolsArray = dataObj["default_symbols"].toArray();
            m_defaultSymbols.clear();
            
            for (const QJsonValue& value : symbolsArray) {
                if (value.isString()) {
                    m_defaultSymbols.append(value.toString());
                }
            }
        }
        
        if (dataObj.contains("data_source") && dataObj["data_source"].isString()) {
            m_dataSource = dataObj["data_source"].toString();
        }
        
        if (dataObj.contains("update_interval") && dataObj["update_interval"].isDouble()) {
            m_updateInterval = dataObj["update_interval"].toInt();
        }
    }
    
    return true;
}

bool ConfigManager::parseInstrumentsConfig(const QJsonDocument& jsonDoc)
{
    if (!jsonDoc.isObject()) {
        qWarning() << "Instruments configuration file root must be an object";
        return false;
    }
    
    QJsonObject root = jsonDoc.object();
    
    // Parse instruments list
    if (root.contains("instruments") && root["instruments"].isArray()) {
        QJsonArray instrumentsArray = root["instruments"].toArray();
        m_instruments.clear();
        
        for (const QJsonValue& value : instrumentsArray) {
            if (!value.isObject()) {
                qWarning() << "Instrument configuration must be an object";
                continue;
            }
            
            QJsonObject instrumentObj = value.toObject();
            InstrumentConfig config;
            
            // Parse required fields
            if (!instrumentObj.contains("symbol") || !instrumentObj["symbol"].isString()) {
                qWarning() << "Instrument configuration missing symbol field or wrong type";
                continue;
            }
            config.symbol = instrumentObj["symbol"].toString();
            
            if (!instrumentObj.contains("name") || !instrumentObj["name"].isString()) {
                qWarning() << "Instrument configuration missing name field or wrong type";
                continue;
            }
            config.name = instrumentObj["name"].toString();
            
            if (!instrumentObj.contains("type") || !instrumentObj["type"].isString()) {
                qWarning() << "Instrument configuration missing type field or wrong type";
                continue;
            }
            config.type = instrumentObj["type"].toString();
            
            if (!instrumentObj.contains("exchange") || !instrumentObj["exchange"].isString()) {
                qWarning() << "Instrument configuration missing exchange field or wrong type";
                continue;
            }
            config.exchange = instrumentObj["exchange"].toString();
            
            // Parse optional fields
            if (instrumentObj.contains("enabled") && instrumentObj["enabled"].isBool()) {
                config.enabled = instrumentObj["enabled"].toBool();
            } else {
                config.enabled = true; // Default enabled
            }
            
            m_instruments.append(config);
        }
    } else {
        qWarning() << "Instruments configuration file missing instruments array";
        return false;
    }
    
    // Parse default instruments list
    if (root.contains("default_instruments") && root["default_instruments"].isArray()) {
        QJsonArray defaultArray = root["default_instruments"].toArray();
        m_defaultInstruments.clear();
        
        for (const QJsonValue& value : defaultArray) {
            if (value.isString()) {
                m_defaultInstruments.append(value.toString());
            }
        }
    } else {
        // If no default list, use first 4 enabled instruments
        m_defaultInstruments.clear();
        int count = 0;
        for (const auto& instrument : m_instruments) {
            if (instrument.enabled && count < 4) {
                m_defaultInstruments.append(instrument.symbol);
                count++;
            }
        }
    }
    
    // Parse update interval
    if (root.contains("update_interval") && root["update_interval"].isDouble()) {
        m_updateInterval = root["update_interval"].toInt();
    }
    
    // Parse maximum instruments count
    if (root.contains("max_instruments") && root["max_instruments"].isDouble()) {
        m_maxInstruments = root["max_instruments"].toInt();
    }
    
    return true;
}

bool ConfigManager::validateGuiConfig() const
{
    // 主配置文件验证 - 只需要确保基本结构正确
    return true;
}

bool ConfigManager::validateInstrumentsConfig() const
{
    if (m_instruments.isEmpty()) {
        qWarning() << "没有配置任何交易对";
        return false;
    }
    
    // 检查默认交易对是否都在配置的交易对列表中
    for (const QString& defaultSymbol : m_defaultInstruments) {
        bool found = false;
        for (const auto& instrument : m_instruments) {
            if (instrument.symbol == defaultSymbol) {
                found = true;
                break;
            }
        }
        if (!found) {
            qWarning() << "默认交易对" << defaultSymbol << "不在配置的交易对列表中";
            return false;
        }
    }
    
    // 检查是否有重复的交易对符号
    QStringList symbols;
    for (const auto& instrument : m_instruments) {
        if (symbols.contains(instrument.symbol)) {
            qWarning() << "发现重复的交易对符号:" << instrument.symbol;
            return false;
        }
        symbols.append(instrument.symbol);
    }
    
    return true;
}

} // namespace btra::gui 