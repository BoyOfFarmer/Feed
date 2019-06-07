/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Ho Van Long
 *
 * Created on May 14, 2019, 3:17 PM
 */


//
// This sample demonstrates the Application class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/AutoPtr.h"
#include <iostream>
#include <sstream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <time.h>
#include "../gen-cpp/NewsFeedService.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class ClientApp : public Application
/// This sample demonstrates some of the features of the Util::Application class,
/// such as configuration file handling and command line arguments processing.
///
/// Try SampleApp --help (on Unix platforms) or SampleApp /help (elsewhere) for
/// more information.
{
public:

    ClientApp() : _helpRequested(false) {
    }

protected:

    void initialize(Application& self) {
        loadConfiguration(); // load default configuration files, if present
        Application::initialize(self);
        // add your own initialization code here
    }

    void uninitialize() {
        // add your own uninitialization code here
        Application::uninitialize();
    }

    void reinitialize(Application& self) {
        Application::reinitialize(self);
        // add your own reinitialization code here
    }

    void defineOptions(OptionSet& options) {
        Application::defineOptions(options);

        options.addOption(
                Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ClientApp>(this, &ClientApp::handleHelp)));

        options.addOption(
                Option("define", "D", "define a configuration property")
                .required(false)
                .repeatable(true)
                .argument("name=value")
                .callback(OptionCallback<ClientApp>(this, &ClientApp::handleDefine)));

        options.addOption(
                Option("config-file", "f", "load configuration data from a file")
                .required(false)
                .repeatable(true)
                .argument("file")
                .callback(OptionCallback<ClientApp>(this, &ClientApp::handleConfig)));

        options.addOption(
                Option("bind", "b", "bind option value to test.property")
                .required(false)
                .repeatable(false)
                .argument("value")
                .binding("test.property"));
    }

    void handleHelp(const std::string& name, const std::string& value) {
        _helpRequested = true;
        displayHelp();
        stopOptionsProcessing();
    }

    void handleDefine(const std::string& name, const std::string& value) {
        defineProperty(value);
    }

    void handleConfig(const std::string& name, const std::string& value) {
        loadConfiguration(value);
    }

    void displayHelp() {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A sample application that demonstrates some of the features of the Poco::Util::Application class.");
        helpFormatter.format(std::cout);
    }

    void defineProperty(const std::string& def) {
        std::string name;
        std::string value;
        std::string::size_type pos = def.find('=');
        if (pos != std::string::npos) {
            name.assign(def, 0, pos);
            value.assign(def, pos + 1, def.length() - pos);
        } else name = def;
        config().setString(name, value);
    }

    int main(const ArgVec& args) {
        if (!_helpRequested) {
            logger().information("Command line:");
            std::ostringstream ostr;
            for (ArgVec::const_iterator it = argv().begin(); it != argv().end(); ++it) {
                ostr << *it << ' ';
            }
            logger().information(ostr.str());
            logger().information("Arguments to main():");
            for (ArgVec::const_iterator it = args.begin(); it != args.end(); ++it) {
                logger().information(*it);
            }
            logger().information("Application properties:");
            printProperties("");
            boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
            boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
            NewsFeedServiceClient client(protocol);
            try {
                transport->open();
                bool exit = false;
                while (!exit) {
                    system("clear");
                    cout << "Choose option you want: " << endl;
                    cout << "1. Create a new Profile\n";
                    cout << "2. Get Profile\n";
                    cout << "3. Create a new NewsFeed\n";
                    cout << "4. Get NewsFeed\n";
                    cout << "5. Update NewsFeed\n";
                    cout << "6. Delete NewsFeed\n";
                    cout << "7. Get ListNewsFeed\n";
                    cout << "8. Exit\n";
                    int cho;
                    cout << "Your choose:  ";
                    cin >> cho;
                    switch (cho) {
                        case 1:
                        {
                            createProfile(client);
                            break;
                        }

                        case 2:
                        {
                            getProfile(client);
                            break;
                        }

                        case 3:
                        {
                            createNewsFeed(client);
                            break;
                        }
                        case 4:
                        {
                            getNewsFeed(client);
                            break;
                        }
                        case 5:
                        {
                            updateNewsFeed(client);
                            break;
                        }
                        case 6:
                        {
                            deleteNewsFeed(client);
                            break;
                        }
                        case 7:
                        {
                            getListNewsFeed(client);
                            break;
                        }
                        case 8:
                        {
                            exit = true;
                            cout << "Exiting ...\n";

                            break;
                        }
                        default:
                        {
                            cout << "Option is not found.\n";
                            getchar();
                        }
                    }
                }
                transport->close();
            } catch (TException& tx) {
                cout << "Error: " << tx.what() << endl;
            }
        }
        return Application::EXIT_OK;
    }

    void printProperties(const std::string& base) {
        AbstractConfiguration::Keys keys;
        config().keys(base, keys);
        if (keys.empty()) {
            if (config().hasProperty(base)) {
                std::string msg;
                msg.append(base);
                msg.append(" = ");
                msg.append(config().getString(base));
                logger().information(msg);

            }
        } else {
            for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it) {
                std::string fullKey = base;
                if (!fullKey.empty()) fullKey += '.';
                fullKey.append(*it);
                printProperties(fullKey);
            }
        }
    }

    bool check(int day, int month, int year) {
        if (month > 12 || month < 1)
            return false;
        if (year < 1900) return false;
        if (day < 1 || day > 31)
            return false;
        if (month == 2) {
            if (day > 29)
                return false;
            if (day == 29) {
                if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                    return true;
                } else return false;
            }
        }
        int b[] = {4, 6, 9, 11};
        for (int i = 0; i < 7; i++)
            if (month == b[i]) {
                if (day == 31)
                    return false;
                break;
            }
        return true;
    }

    void createProfile(NewsFeedServiceClient client) {
        system("clear");
        string name;
        cout << "Create a new Profile:\n";
        cout << "Enter username: ";
        getchar();
        getline(cin, name);
        cout << "Enter your gender:\n";
        cout << "\t1. Male     2. Female   3. Other\n";
        int gender;
        cin>>gender;

        if (gender < 1 || gender > 3) {
            cout << "Illegal gender\n";
            getchar();
            getchar();
            return;
        }
        cout << "Enter your birthday:\n";
        int day, month, year;
        cout << "\tday:   ";
        cin>>day;
        cout << "\tmonth:   ";
        cin>>month;
        cout << "\tyear:   ";
        cin>>year;
        bool ok = check(day, month, year);
        if (!ok || year > 2020) {
            cout << "Illegal birthday" << endl;
            getchar();
            getchar();
            return;
        }
        tm *tmp = new tm();
        tmp->tm_mday = day;
        tmp->tm_mon = month - 1;
        tmp->tm_year = year - 1900;
        int now = time(0);
        int birthday = mktime(tmp);
        
        if (now < birthday) {
            cout << "Illegal birthday" << endl;
            getchar();
            getchar();
            return;
        }

        ProfileCreateResult result;
        client.createProfile(result, name, gender, birthday);
        if (!result.exitCode) {
            cout << "Your ID: " << result.result << endl;
        } else {
            cout << "Error: " << result.message << endl;
        }
        getchar();
        getchar();
    }

    string stringGender(int gender) {
        if (gender == 1)
            return "Male";
        if (gender == 2)
            return "Female";
        return "Other";
    }

    void getProfile(NewsFeedServiceClient client) {
        system("clear");
        cout << "Get a Profile\n";
        cout << "Enter id of Profile: ";
        int id;
        cin>>id;

        ProfileResult res;
        client.getProfile(res, id);
        if (!res.exitCode) {    
            cout << "Profile of User have id: " << id << endl;
            cout << "Name:    " << res.result.name << endl;
            cout << "Gender:  " << stringGender(res.result.gender) << endl;
            time_t con = (time_t) res.result.birthday;
            tm *tmp = localtime(&con);
            cout << "Birthday:" << tmp->tm_mday << "/" << tmp->tm_mon + 1 << "/" << tmp->tm_year + 1900 << endl;
        }else {
            cout << "Error: " << res.message << endl;
        } 
        getchar();
        getchar();
    }

    void createNewsFeed(NewsFeedServiceClient client) {
        system("clear");
        cout<<"Create a NewsFeed:\n";
        int owner;
        string content;
        
        int date_time;
        cout << "Enter username's ID: " ;
        cin>>owner;
        cout << "Enter content: ";
        getchar();
        getline(cin, content);
        cout << "Enter status:\n";
        cout << "\t1. Public     2. Private\n";
        int status;
        cin>>status;
        if (status < 1 || status > 2) {
            cout << status<< " Illegal status\n";
            getchar();
            getchar();
            return;
        }
        
        FeedCreateResult res;
        client.createNewsFeed(res, owner, content, status);
        if (!res.exitCode) {
            cout << "Your feed ID: " << res.result << endl;
        } else {
            cout << "Error: " << res.message << endl;
        }
        getchar();
        getchar();
    }
    
    string stringStatus(int status) {
        if (status == 1)
            return "Public";
        if (status == 2)
            return "Private";
        return "None";
    }
    
    void getNewsFeed(NewsFeedServiceClient client) {
        system("clear");
        cout<<"Get NewsFeed:\n";
        int id;
        cout << "Enter your NewsFeed's ID: ";
        cin>>id;
        FeedResult result;
        client.getFeed(result, id);
        if (!result.exitCode) {
            time_t con = (time_t) result.result.edit_time;
            tm *tmp = localtime(&con);
            cout <<"Owner's Id: "<<result.result.owner <<"\nContent:   " << result.result.content << "\nStatus:    " << stringStatus(result.result.status) << "\nTime edit: " << tmp->tm_mday << "/" << tmp->tm_mon + 1 << "/" << tmp->tm_year+1900  << endl;
        } else {
            cout << "Error: " << result.message << endl;
        }
        getchar();
        getchar();
    }

    void getListNewsFeed(NewsFeedServiceClient client) {
        system("clear");
        cout<<"Get list NewFeed\n";
        int owner;
        cout << "Enter your ID:";;
        cin>>owner;
        FeedCountResult user;
        client.getFeedCount(user, owner);
        if (user.exitCode) {
            //Error user not found
            cout <<user.exitCode<<"Error: " <<user.message << endl;
            getchar();
            getchar();
            return;
        }
        int count = 3;
        
        Item post=user.result;
        if (post.post<0){
            cout<<"Have no feed\n";
        } else
        while (post.post>=0) {
            ListFeedResult res;
            client.getListFeed(res, owner, post, count);
            for (int i = 0; i < res.result.count; i++) {
                time_t con = (time_t) res.result.feedlist[i].edit_time;
                tm *tmp = localtime(&con);
                cout <<"NewsFeed's ID:  "<< res.result.feedlist[i].id <<"\nContent:   " << res.result.feedlist[i].content << "\nStatus:    " << stringStatus(res.result.feedlist[i].status) << "\nTime edit: " << tmp->tm_mday << "/" << tmp->tm_mon + 1 << "/" << tmp->tm_year+1900  << endl<< endl;
            }
            post=res.result.nex;
            if (post.post>=0) {
                cout<<"Choose option:\n";
                cout << "\t1.     See more ..." << endl;
                cout << "\t2.     Exit ..." << endl;
                int choose;
                cin>>choose;
                if (choose == 2) {
                    return;
                }
            } else {
                cout << "Have no newsfeed else";
               
                break;
            }
        }
        getchar();
        getchar();
    }

    void updateNewsFeed(NewsFeedServiceClient client) {
        system("clear");
        cout<<"Update NewsFeed:\n";
        int owner;
        string content;
        int status;

        int id;
        cout << "Enter NewsFeed's ID:";
        cin>>id;
        cout << "Enter content:" ;
        getchar();
        getline(cin, content);
        cout << "Enter status:\n";
        cout << "\t1. Public     2. Private\n";
        cin>>status;
        if (status < 1 || status > 2) {
            cout << status<< " Illegal status\n";
            getchar();
            getchar();
            return;
        }
        FeedUpdateResult res;
        client.updateNewsFeed(res, id, content, status);
        if (!res.exitCode) {
            cout << "Update Success" << endl;
        } else {
            cout <<"Error: " <<res.message << endl;
        }
        getchar();
        getchar();
    }

    void deleteNewsFeed(NewsFeedServiceClient client) {
        int owner;
        system("clear");
        cout << "Enter username's ID:" << endl;
        cin>>owner;
        int id;
        cout << "Enter NewsFeed's ID:" << endl;
        cin>>id;
        FeedDeleteResult res;
        client.deleteNewsFeed(res, id, owner);
        if (!res.exitCode) {
            cout << "Delete Success" << endl;
        } else {
            cout <<"Error: " <<res.message << endl;
        }
        getchar();
        getchar();
    }

private:
    bool _helpRequested;
};


POCO_APP_MAIN(ClientApp)
