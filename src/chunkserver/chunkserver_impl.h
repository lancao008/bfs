// Copyright (c) 2014, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: yanshiguang02@baidu.com

#ifndef  BFS_TRUNKSERVER_IMPL_H_
#define  BFS_TRUNKSERVER_IMPL_H_

#include "proto/chunkserver.pb.h"
#include "proto/nameserver.pb.h"

#include <common/thread_pool.h>

namespace sofa {
namespace pbrpc {
class HTTPRequest;
class HTTPResponse;
}
}

namespace baidu {
namespace bfs {

class BlockManager;
class RpcClient;
class NameServer_Stub;
class ChunkServer_Stub;
class Block;
class CounterManager;

class ChunkServerImpl : public ChunkServer {
public:
    ChunkServerImpl();
    virtual ~ChunkServerImpl();
    void Register();
    void SendHeartbeat();
    void SendBlockReport();
    bool ReportFinish(Block* block);

    virtual void WriteBlock(::google::protobuf::RpcController* controller,
                            const WriteBlockRequest* request,
                            WriteBlockResponse* response,
                            ::google::protobuf::Closure* done);
    virtual void ReadBlock(::google::protobuf::RpcController* controller,
                           const ReadBlockRequest* request,
                           ReadBlockResponse* response,
                           ::google::protobuf::Closure* done);
    virtual void GetBlockInfo(::google::protobuf::RpcController* controller,
                              const GetBlockInfoRequest* request,
                              GetBlockInfoResponse* response,
                              ::google::protobuf::Closure* done);
    bool WebService(const sofa::pbrpc::HTTPRequest& request,
                    sofa::pbrpc::HTTPResponse& response);
private:
    void LogStatus(bool routine);
    void WriteNext(const std::string& next_server,
                   ChunkServer_Stub* stub,
                   const WriteBlockRequest* next_request,
                   WriteBlockResponse* next_response,
                   const WriteBlockRequest* request,
                   WriteBlockResponse* response,
                   ::google::protobuf::Closure* done);
    void WriteNextCallback(const WriteBlockRequest* next_request,
                           WriteBlockResponse* next_response,
                           bool failed, int error,
                           const std::string& next_server,
                           std::pair<const WriteBlockRequest*, WriteBlockResponse*> origin,
                           ::google::protobuf::Closure* done,
                           ChunkServer_Stub* stub);
    void LocalWriteBlock(const WriteBlockRequest* request,
                         WriteBlockResponse* response,
                         ::google::protobuf::Closure* done);
    void RemoveObsoleteBlocks(std::vector<int64_t> blocks);
    void PullNewBlocks(std::vector<ReplicaInfo> new_replica_info);
    void GatherCounter();
    void StopBlockReport();
private:
    BlockManager*   block_manager_;
    std::string     data_server_addr_;
    RpcClient*      rpc_client_;
    ThreadPool*     work_thread_pool_;
    ThreadPool*     read_thread_pool_;
    ThreadPool*     write_thread_pool_;
    ThreadPool*     heartbeat_thread_;
    NameServer_Stub* nameserver_;
    int32_t chunkserver_id_;
    CounterManager* counter_manager_;
    int64_t heartbeat_task_id_;
    volatile int64_t blockreport_task_id_;
    volatile int blockreport_status_;
};

} // namespace bfs
} // namespace baidu

#endif  //__TRUNKSERVER_IMPL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
