#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <qtmetamacros.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include "core/main_cfg.h"

namespace btra::gui {

/**
 * @brief Instrument configuration information structure
 */
struct InstrumentConfig {
    QString symbol;   ///< Instrument symbol
    QString name;     ///< Instrument name
    QString type;     ///< Instrument type
    QString exchange; ///< Exchange
    bool enabled;     ///< Whether enabled
};

/**
 * @brief Configuration manager class
 *
 * Responsible for reading and managing guiconfig.json and instruments.json configuration files
 */
class ConfigManager : public QObject {
    Q_OBJECT

public:
    explicit ConfigManager(QObject* parent = nullptr);
    ~ConfigManager();

    /**
     * @brief Load main configuration file
     * @param configPath Main configuration file path
     * @return Whether loading was successful
     */
    bool loadConfig(const QString& configPath = "guiconfig.json");

    QString get_core_config_file() const { return core_config_file_; }

    /**
     * @brief Load instruments configuration file
     * @param instrumentsPath Instruments configuration file path
     * @return Whether loading was successful
     */
    bool loadInstrumentsConfig(const QString& instrumentsPath);

    /**
     * @brief Get list of all enabled instrument symbols
     * @return List of instrument symbols
     */
    Q_INVOKABLE QStringList getEnabledInstruments() const;

    /**
     * @brief Get list of default instrument symbols
     * @return List of default instrument symbols
     */
    QStringList getDefaultInstruments() const;

    /**
     * @brief Get all instrument configuration information
     * @return List of instrument configurations
     */
    QVariantList getAllInstruments() const;

    /**
     * @brief Get configuration information for specified instrument
     * @param symbol Instrument symbol
     * @return Instrument configuration information
     */
    QVariantMap getInstrumentConfig(const QString& symbol) const;

    /**
     * @brief Check if instrument is enabled
     * @param symbol Instrument symbol
     * @return Whether enabled
     */
    bool isInstrumentEnabled(const QString& symbol) const;

    /**
     * @brief Get update interval
     * @return Update interval (milliseconds)
     */
    int getUpdateInterval() const;

    /**
     * @brief Get maximum number of instruments
     * @return Maximum number of instruments
     */
    int getMaxInstruments() const;

    /**
     * @brief Get theme
     * @return Theme name
     */
    QString getTheme() const;

    /**
     * @brief Get language
     * @return Language code
     */
    QString getLanguage() const;

    /**
     * @brief Get auto refresh setting
     * @return Whether auto refresh is enabled
     */
    bool getAutoRefresh() const;

    /**
     * @brief Get refresh interval
     * @return Refresh interval (milliseconds)
     */
    int getRefreshInterval() const;

    /**
     * @brief Get default timeframe
     * @return Default timeframe
     */
    QString getDefaultTimeframe() const;

    /**
     * @brief Get maximum number of candlesticks
     * @return Maximum number of candlesticks
     */
    int getMaxCandlesticks() const;

    /**
     * @brief Get default zoom factor
     * @return Default zoom factor
     */
    double getDefaultZoomFactor() const;

    /**
     * @brief Get default symbols list
     * @return Default symbols list
     */
    QStringList getDefaultSymbols() const;

    /**
     * @brief Get data source
     * @return Data source name
     */
    QString getDataSource() const;

    /**
     * @brief Reload configuration file
     * @return Whether reload was successful
     */
    bool reloadConfig();

    bool get_simulation() const { return simulation_; }

    const MainCfg &get_core_engine_cfg() const { return core_engine_cfg_; }

signals:
    /**
     * @brief Configuration loaded signal
     */
    void configLoaded();

    /**
     * @brief Configuration load error signal
     * @param error Error message
     */
    void configLoadError(const QString& error);

private:
    /**
     * @brief Parse main configuration file
     * @param jsonDoc JSON document
     * @return Whether parsing was successful
     */
    bool parseGuiConfig(const QJsonDocument& jsonDoc);

    /**
     * @brief Parse instruments configuration file
     * @param jsonDoc JSON document
     * @return Whether parsing was successful
     */
    bool parseInstrumentsConfig(const QJsonDocument& jsonDoc);

    /**
     * @brief Validate main configuration data
     * @return Whether validation passed
     */
    bool validateGuiConfig() const;

    /**
     * @brief Validate instruments configuration data
     * @return Whether validation passed
     */
    bool validateInstrumentsConfig() const;

private:
    QString m_configPath;                  ///< Main configuration file path
    QString m_instrumentsPath;             ///< Instruments configuration file path
    QString core_config_file_;             /*Core engine configuration file path*/
    QList<InstrumentConfig> m_instruments; ///< List of instrument configurations
    QStringList m_defaultInstruments;      ///< Default instruments list
    int m_updateInterval;                  ///< Update interval
    int m_maxInstruments;                  ///< Maximum number of instruments
    bool m_configLoaded;                   ///< Whether configuration is loaded

    // GUI configuration
    QString m_theme;              ///< Theme
    QString m_language;           ///< Language
    bool m_autoRefresh;           ///< Auto refresh setting
    int m_refreshInterval;        ///< Refresh interval
    QString m_defaultTimeframe;   ///< Default timeframe
    int m_maxCandlesticks;        ///< Maximum number of candlesticks
    double m_defaultZoomFactor;   ///< Default zoom factor
    QStringList m_defaultSymbols; ///< Default symbols list
    QString m_dataSource;         ///< Data source

    bool simulation_{false};
    MainCfg core_engine_cfg_;
};

} // namespace btra::gui

#endif // CONFIGMANAGER_H