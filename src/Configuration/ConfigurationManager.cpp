#include "FileTypes/INIFile/ConfFile.hpp"
#include "Configuration/ConfDatabase.hpp"
#include "Configuration/ConfigurationManager.hpp"

Configuration::ConfDatabase Configuration::ConfigurationManager::dbConf;

void Configuration::ConfigurationManager::Load() {
	Logger::log("CONF", "Loading configuration files...");
	dbConf.Load();
}

void Configuration::ConfigurationManager::Reload() {
	Logger::log("CONF", "Reloading configuration files...");
	dbConf.Reload();
}

void Configuration::ConfigurationManager::Save() {
	Logger::log("CONF", "Saving configuration files...");
	dbConf.Save();
}