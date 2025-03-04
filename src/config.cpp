#include "config.h"

firewallType Config::setFirewallType(const std::string &firewallString)
{
    if(firewallString == "mock") return firewallType::mock;
    if(firewallString == "ufw") return firewallType::ufw;
    if(firewallString == "firewalld") return firewallType::firewalld;
    if(firewallString == "iptables") return firewallType::iptables; 
    throw std::invalid_argument("Invalid firewall type selected");
}

void Config::populateSequencesKnockPorts(const std::string &key, const std::string &value)
{
    auto knockerPorts = utility::parseCSV(value);
    sequences.try_emplace(key, Sequence{});
    sequences.at(key).clearKnockerPorts(); //Wipe the existing KnockerPort settings
    for(auto &port : knockerPorts){
        int numberPort{-1};
        try{
            numberPort = std::stoi(port);
        } catch (std::invalid_argument const& ex){
            throw std::invalid_argument{"port " + port + " is not a valid number"}; 
        } catch (std::out_of_range const& ex) {
            throw std::invalid_argument{"port " + port +" is too big a number"};
        }
        sequences.at(key).addPortToSequence(numberPort);
    }
}

void Config::populateSequencesUnlockPorts(const std::string &key, const std::string &port)
{
    const auto portDef = utility::parseSlash(port);
    if(portDef.second.empty()) throw std::invalid_argument{"Missing / in " + key + "_unlock"};
    sequences.try_emplace(key, Sequence{});
    int numberPort{-1};
    try{
        numberPort = std::stoi(port);
    } catch (std::invalid_argument const& ex){
        throw std::invalid_argument{"unlock port " + port + " is not a valid number"}; 
    } catch (std::out_of_range const& ex) {
        throw std::invalid_argument{"unlockPort " + port +" is too big a number"};
    }
    sequences.at(key).setunlockPort(numberPort, (portDef.second == "tcp"));
}

std::vector<int> Config::dumpPorts() const
{
    std::vector<int> allPorts{};
    for (auto [_, seq]: sequences){
        if (!seq.isTcp()) allPorts.push_back(seq.getUnlockPort());
        std::copy(seq.getKnockPorts().begin(), seq.getKnockPorts().end(), std::back_inserter(allPorts));
    }
    return allPorts;

}

void Config::validate()
{
    if (secret_key.empty()) throw std::invalid_argument{"`secret_key` is missing from config."};
    if (log_file.empty())  throw std::invalid_argument{"`log_file` is missing from config."};
    if(firewall == firewallType::invalid)  throw std::invalid_argument{"`firewall` is missing from config or invalid type."};
    if(ban_timer == -1 && ban) throw std::invalid_argument{"`ban_timer` is missing from config."};
    if(sequences.empty()) throw std::invalid_argument{"Missing _sequence or _unlock code" };
    for(auto [key, seq] : sequences){
        if(!seq.isValid()) throw std::invalid_argument{"Invalid " + key + "_unlock or " + key + "_sequence"};
    }
    auto allPorts = dumpPorts();
    std::unordered_set<int> dupCheck{};
    for (int port : allPorts) {
        if (dupCheck.find(port) != dupCheck.end()) throw std::invalid_argument("Duplicate port found in knockerSequence or UDP unlock port");
        dupCheck.insert(port);
    }
    valid = true;
}

Config::Config(const std::string &filePath) : secret_key{}, log_file{""}, firewall{firewallType::invalid}, ban{false}, ban_timer{-1}, sequences{}, valid{false}
{
    std::ifstream file {filePath};
    if (!file) throw std::invalid_argument("Cannot open the config file at: " + filePath + ".");
    std::string line{};
    while (std::getline(file, line)) {
        auto comment_start = line.find("#");
        if (comment_start != std::string::npos){ // comment found
          line = line.substr(0, comment_start); //remove comment section
        }
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        utility::ltrim(key);
        utility::rtrim(key);
        utility::ltrim(value);
        utility::rtrim(value);
        
        //parse the different keys
        if (key == "secret_key") secret_key = value;
        else if (key == "log_file") {
                log_file = value;
                std::ofstream log{value, std::ios::app};
                if(!log.good()) throw std::invalid_argument("Cannot open the log file at: " + value + ".");
        }
        else if (key == "firewall") firewall = setFirewallType(value);
        else if (key == "ban") {
            if (value != "true" && value != "false") throw std::invalid_argument{"ban value is incorrect!"};
            ban = (value == "true");
        }
        else if (key == "ban_timer") {
            try{
                ban_timer = std::stoi(value);
            } catch (std::invalid_argument const& ex){
                throw std::invalid_argument{"ban_timer is not a valid number"}; 
            } catch (std::out_of_range const& ex) {
                throw std::invalid_argument{"ban_timer number is too big"};
            }
            if (ban_timer < 0) throw std::invalid_argument{"ban_timer must be a positive number"}; 
        }
        else if (key.find("_sequence") != std::string::npos) {
            auto service = key.substr(0,key.find("_sequence"));
            populateSequencesKnockPorts(service,value);
        }
        else if (key.find("_unlock") != std::string::npos) {
            auto service = key.substr(0,key.find("_unlock"));
            populateSequencesUnlockPorts(service,value);
        }
        else throw std::invalid_argument{"Unknown value of: " + key};
    }
    validate();
}

std::string Config::getSecretKey() const{ return secret_key;}

std::string Config::getLogFile() const{ return log_file;}

firewallType Config::getFirewall() const{ return firewall;}

bool Config::banEnabled() const{ return ban; }

int Config::getBanTimer() const{ return ban_timer; }

std::unordered_map<std::string, Sequence> Config::getSequences() const{ return sequences;}

bool Config::isValid() const { return valid; }
