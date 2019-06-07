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
// SampleServer.cpp
//
// This sample demonstrates the ServerApplication class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/DateTimeFormatter.h"
#include <iostream>

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <thrift/TToString.h>
#include <stdlib.h>

#include <boost/make_shared.hpp>
#include <stdexcept>
#include <sstream>
#include <kcpolydb.h>
#include <time.h>
#include "../gen-cpp/NewsFeedService.h"
using namespace std;
using namespace kyotocabinet;

using Poco::Util::Application;
using Poco::Util::Subsystem;
using Poco::Util::ServerApplication;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::Task;
using Poco::TaskManager;
using Poco::DateTimeFormatter;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace apache::thrift::concurrency;
using ::apache::thrift::server::TSimpleServer;

using boost::shared_ptr;
static int index_user = 0;
static int index_feed = 0;

class DbSystem : public Subsystem {
private:
    static DbSystem* ins;
    PolyDB *dbuser, *dbfeed, *dbinfo, *dbpost;
public:

    DbSystem() {
    }

    ~DbSystem() {

    }



    static DbSystem* getInstance();
    static DbSystem* deleteInstance();

    void setInfo(string key, string value) {
        if (!dbinfo->set(key, value)) {
            cerr << "set error: " << dbinfo->error().name() << endl;
        }
    }

    string getInfo(string key) {
        string value;
        if (dbinfo->get(key, &value)) {
            //cout << "value: " << value << endl;
            return value;
        } else {
            cerr << "get error: " << dbinfo->error().name() << endl;
            return "0";
        }

    }

    void setPost(string key, string value) {
        if (!dbpost->set(key, value)) {
            cerr << "set error: " << dbpost->error().name() << endl;
        }
    }

    string getPost(string key) {
        string value;
        if (dbpost->get(key, &value)) {
            return value;
        } else {
            cerr << "get error: " << dbpost->error().name() << endl;
            return "0";
        }

    }

    void setUser(string key, string value) {
        if (!dbuser->set(key, value)) {
            cerr << "set error: " << dbuser->error().name() << endl;
        }
    }

    string getUser(string key) {
        string value;
        if (dbuser->get(key, &value)) {
            //cout << "value: " << value << endl;
            return value;
        } else {
            cerr << "get error: " << dbuser->error().name() << endl;
            return "0";
        }

    }

    void setFeed(string key, string value) {
        if (!dbfeed->set(key, value)) {
            cerr << "set error: " << dbfeed->error().name() << endl;
        }
    }

    string getFeed(string key) {
        string value;
        if (dbfeed->get(key, &value)) {
            //cout << "value: " << value << endl;
            return value;
        } else {
            cerr << "get error: " << dbfeed->error().name() << endl;
            return "0";
        }
    }

    string removeFeed(string key) {
        if (!dbfeed->remove(key)) {
            cerr << "remove error: " << dbfeed->error().name() << endl;
            return "0";
        }
        return "1";
    }
protected:

    void initialize(Application& self) {
        cout << "Start open database" << endl;
        // Open info database
        this->dbinfo = new PolyDB;
        if (!this->dbinfo->open("dbinfo.kch", PolyDB::OWRITER | PolyDB::OCREATE)) {
            cerr << "open info error: " << this->dbinfo->error().name() << endl;
        }
        // Open user database

        this->dbuser = new PolyDB;
        if (!this->dbuser->open("dbuser.kch", PolyDB::OWRITER | PolyDB::OCREATE)) {
            cerr << "open user error: " << this->dbuser->error().name() << endl;
        }
        // Open newsfeed database
        this->dbfeed = new PolyDB;
        if (!this->dbfeed->open("dbfeed.kch", PolyDB::OWRITER | PolyDB::OCREATE)) {
            cerr << "open feed error: " << this->dbfeed->error().name() << endl;
        }
        // Open post list database
        this->dbpost = new PolyDB;
        if (!this->dbpost->open("dbpost.kch", PolyDB::OWRITER | PolyDB::OCREATE)) {
            cerr << "open post error: " << this->dbpost->error().name() << endl;
        }
        index_user = stoi(DbSystem::getInstance()->getInfo("User"));
        index_feed = stoi(DbSystem::getInstance()->getInfo("Feed"));
    }

    void uninitialize() {
        cout << "Close database" << endl;
        //Close post list database
        if (!this->dbpost->close()) {
            cerr << "close post list error: " << this->dbpost->error().name() << endl;
        }
        // Close user database
        if (!this->dbuser->close()) {
            cerr << "close user error: " << this->dbuser->error().name() << endl;
        }
        // Close newsfeed database
        if (!this->dbfeed->close()) {
            cerr << "close newsfeed error: " << this->dbfeed->error().name() << endl;
        }

        // Close info database
        if (!this->dbinfo->close()) {
            cerr << "close info error: " << this->dbinfo->error().name() << endl;
        }
        delete this->dbuser;
        delete this->dbfeed;
        delete this->dbpost;
        delete this->dbinfo;
    }

    const char* name() const {
        return "DB-SYSTEM";
    }
};
DbSystem* DbSystem::ins = NULL;

DbSystem* DbSystem::getInstance() {
    if (ins == NULL) {
        ins = new DbSystem();
    }
    return ins;
}

DbSystem* DbSystem::deleteInstance() {
    if (ins != NULL) {
        delete ins;
        ins = NULL;
    }
    return ins;
}

class CacheSystem : public Subsystem {
private:
    static CacheSystem *ins;


    list<int> cacheuser;
    list<int> cachefeed;
    unordered_map<int, list<int>::iterator> mapuser;
    unordered_map<int, list<int>::iterator> mapfeed;
    int sizeuser;
    int sizefeed;

public:

    CacheSystem() {

    }

    ~CacheSystem() {

    }

    static CacheSystem * getInstance() {
        if (ins == NULL) {
            ins = new CacheSystem();
        }
        return ins;
    }

    void setUser(string key, string value) {

    }

    string getUser(string key) {

    }

    void setFeed(string key, string value) {

    }

    string getFeed(string key) {

    }
protected:

    void initialize(Application& self) {
        cout << "Start open cache" << endl;

    }

    void uninitialize() {
        cout << "Close cache" << endl;
    }

    const char* name() const {
        return "CACHE-SYSTEM";
    }
};

class NewsFeedServiceHandler : virtual public NewsFeedServiceIf {
public:

    NewsFeedServiceHandler() {
        // Your initialization goes here
    }

    void createProfile(ProfileCreateResult& _return, const std::string& name, const int8_t gender, const int32_t birthday) {
        // Your implementation goes here
        printf("createUser\n");
        int id;
        //m.lock();
        id = ++index_user;

        //m.unlock();
        DbSystem::getInstance()->setInfo("User", to_string(index_user));
        Profile user;
        user.name = name;
        user.id = id;
        user.gender = gender;
        user.birthday = birthday;
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        user.write(protocol.get());
        DbSystem::getInstance()->setUser(to_string(id), transport->getBufferAsString());

        PostList post;
        post.id = id;
        post.count = 0;
        boost::shared_ptr<TMemoryBuffer> transport2(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol2(new TBinaryProtocol(transport2));
        post.write(protocol2.get());
        DbSystem::getInstance()->setPost(to_string(id), transport2->getBufferAsString());


        _return.exitCode = 0;
        _return.result = id;
    }

    void getProfile(ProfileResult& _return, const int32_t id) {
        // Your implementation goes here
        printf("getUser\n");
        string value = DbSystem::getInstance()->getUser(to_string(id));
        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "Profile not exist";
            return;
        }
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        Profile user;
        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        user.read(protocol.get());
        _return.result = user;
        _return.exitCode = 0;
        //_return.message ="";

    }

    void createNewsFeed(FeedCreateResult& _return, const int32_t owner, const std::string& content, const int8_t status) {
        // Your implementation goes here
        printf("createNewsFeed\n");
        int id;
        //m.lock();
        id = ++index_feed;
        //m.unlock();
        DbSystem::getInstance()->setInfo("Feed", to_string(index_feed));
        // write feed
        NewsFeed feed;
        feed.id = id;
        feed.owner=owner;
        feed.content = content;
        feed.edit_time = time(0);
        feed.status = status;
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        feed.write(protocol.get());
        DbSystem::getInstance()->setFeed(to_string(id), transport->getBufferAsString());
        // read post and write id to post.feedList
        boost::shared_ptr<TMemoryBuffer> transport1(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));

        PostList post;
        string value = DbSystem::getInstance()->getPost(to_string(owner));
        transport1->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        post.read(protocol1.get());
        post.listFeed.push_back(id);
        post.count++;
        cout<<"post count"<<post.count;
        boost::shared_ptr<TMemoryBuffer> transport2(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol2(new TBinaryProtocol(transport2));
        post.write(protocol2.get());
        DbSystem::getInstance()->setPost(to_string(owner), transport2->getBufferAsString());

        _return.exitCode = 0;
        _return.result = id;
    }

    void updateNewsFeed(FeedUpdateResult& _return, const int32_t id, const std::string& content, const int8_t status) {
        // Your implementation goes here
        printf("updateNewsFeed\n");
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        NewsFeed feed;

        string value = DbSystem::getInstance()->getFeed(to_string(id));

        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "NewsFeed not exist";
            return;
        }

        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        feed.read(protocol.get());
        feed.content = content;
        feed.edit_time = time(0);
        feed.status = status;
        boost::shared_ptr<TMemoryBuffer> transport1(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));
        feed.write(protocol1.get());
        DbSystem::getInstance()->setFeed(to_string(id), transport1->getBufferAsString());

        _return.exitCode = 0;

    }

    void deleteNewsFeed(FeedDeleteResult& _return, const int32_t id, const int32_t owner) {
        // Your implementation goes here
        printf("deleteNewsFeed\n");
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        // remove feed
        string value = DbSystem::getInstance()->removeFeed(to_string(id));
        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "NewsFeed not exist";
            return;
        }
        // read post and delete id to post.feedList
        PostList user;
        value = DbSystem::getInstance()->getPost(to_string(owner));
        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        user.read(protocol.get());
        for (int i = 0; i < user.count; i++) {
            if (user.listFeed[i] == id) {
                user.listFeed.erase(user.listFeed.begin() + i);
                break;
            }
        }
        boost::shared_ptr<TMemoryBuffer> transport1(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));
        user.count--;
        user.write(protocol1.get());
        DbSystem::getInstance()->setPost(to_string(owner), transport1->getBufferAsString());
        _return.exitCode = 0;

    }

    void getFeed(FeedResult& _return, const int32_t id) {
        // Your implementation goes here
        printf("getFeed\n");
        string value = DbSystem::getInstance()->getFeed(to_string(id));

        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "NewsFeed not exist";
            return;
        }
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        NewsFeed feed;
        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        feed.read(protocol.get());
        _return.result = feed;
        _return.exitCode = 0;

        //_return.message = "";

    }

    void getFeedCount(FeedCountResult& _return, const int32_t owner) {
        // Your implementation goes here
        printf("getFeedCount\n");
        string value = DbSystem::getInstance()->getPost(to_string(owner));
        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "User not exist";
            return;
        }

        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        PostList user;
        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        user.read(protocol.get());
        _return.exitCode = 0;
        _return.result.post = user.count-1;
        if (user.count>0)
            _return.result.id = user.listFeed[user.count-1];
        else _return.result.id = -1;
        
        
    }

    void getListFeed(ListFeedResult& _return, const int32_t owner, const Item& post, const int32_t count) {
        string value = DbSystem::getInstance()->getPost(to_string(owner));
        printf("getListFeed\n");
        if (value == "0") {
            _return.exitCode = 404;
            _return.message = "User not exist";
            return;
        }
        boost::shared_ptr<TMemoryBuffer> transport(new TMemoryBuffer());
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        PostList user;
        transport->resetBuffer((uint8_t*) value.data(), static_cast<uint32_t> (value.length()));
        user.read(protocol.get());
        int cou = 0;
        int pos=post.post;
        while (cou<count && pos>=0)   {
            boost::shared_ptr<TMemoryBuffer> transport1(new TMemoryBuffer());
            boost::shared_ptr<TProtocol> protocol1(new TBinaryProtocol(transport1));
            NewsFeed feed;
            string value1 = DbSystem::getInstance()->getFeed(to_string(user.listFeed[pos]));
            transport1->resetBuffer((uint8_t*) value1.data(), static_cast<uint32_t> (value1.length()));
            feed.read(protocol1.get());
            if (feed.id<=post.id){
                _return.result.feedlist.push_back(feed);
                cou++;
                 _return.result.nex.id=feed.id-1;
            }
            pos--;
        }
        
        _return.exitCode = 0;
        _return.message = "";
        _return.result.count = cou;
        _return.result.nex.post=pos;
        if (pos<0){
            _return.result.nex.id=-1;
        }
           
  
    }


};

class ServerTask : public Task {
public:

    ServerTask() : Task("ServerTask") {
    }

    void runTask() {
        Application& app = Application::instance();
        //        //		while (!sleep(5000))
        //        {
        //            Application::instance().logger().information("busy doing nothing... " + DateTimeFormatter::format(app.uptime()));
        //        }
        int port = 9090;
        boost::shared_ptr<NewsFeedServiceHandler> handler(new NewsFeedServiceHandler());
        boost::shared_ptr<TProcessor> processor(new NewsFeedServiceProcessor(handler));
        boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
        boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
        boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());



        TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
        cout << "Starting the server..." << endl;
        server.serve();
        //server.stop();
    }
};

class SampleServer : public ServerApplication {
public:

    SampleServer() : _helpRequested(false) {

    }

    ~SampleServer() {
    }

protected:

    void initialize(Application& self) {
        loadConfiguration(); // load default configuration files, if present
        addSubsystem(DbSystem::getInstance());
        addSubsystem(new CacheSystem());

        ServerApplication::initialize(self);

        logger().information("starting up");
    }

    void uninitialize() {
        logger().information("shutting down");
        ServerApplication::uninitialize();

    }

    void defineOptions(OptionSet& options) {
        ServerApplication::defineOptions(options);

        options.addOption(
                Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<SampleServer>(this, &SampleServer::handleHelp)));
    }

    void handleHelp(const std::string& name, const std::string& value) {
        _helpRequested = true;
        displayHelp();
        stopOptionsProcessing();
    }

    void displayHelp() {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A sample server application that demonstrates some of the features of the Util::ServerApplication class.");
        helpFormatter.format(std::cout);
    }

    int main(const ArgVec& args) {
        if (!_helpRequested) {

            TaskManager tm;
            tm.start(new ServerTask);
            waitForTerminationRequest();
            tm.cancelAll();
            tm.joinAll();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};


POCO_SERVER_MAIN(SampleServer)
