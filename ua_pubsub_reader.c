/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2017-2018 Fraunhofer IOSB (Author: Andreas Ebner)
 * Copyright (c) 2019 Fraunhofer IOSB (Author: Julius Pfrommer)
 * Copyright (c) 2019 Kalycito Infotech Private Limited
 */
//sourcetimestamp 관련 헤더파일
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/securitypolicy.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <stdlib.h>
#include <sys/timeb.h>
#include <time.h>
#include <signal.h>
//sourcetimestamp 관련 헤더파일 끝


#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_pubsub.h>
#include "server/ua_server_internal.h"
#include <stdlib.h>
#include <open62541/types.h>

#include <stdbool.h>
#define bool _Bool

#ifdef UA_ENABLE_PUBSUB /* conditional compilation */

#include "ua_pubsub.h"

#ifdef UA_ENABLE_PUBSUB_INFORMATIONMODEL
#include "ua_pubsub_ns0.h"
#endif

#ifdef UA_ENABLE_PUBSUB_DELTAFRAMES
#include "ua_types_encoding_binary.h"
#endif

#define UA_MAX_SIZENAME 64  /* Max size of Qualified Name of Subscribed Variable */

//RPI001
float FloatRPITemp;
float FloatRPITemp2 = 0;

float FloatRPIHumi;
float FloatRPIHumi2 = 0;

float FloatRPIGas;
float FloatRPIGas2 = 0;

uint32_t UIntRPIIR;
uint32_t UIntRPIIR2 = 0;

uint32_t UIntRPISound;
uint32_t UIntRPISound2 = 0;

double DoubleRPIAccelX;
double DoubleRPIAccelX2 = 0;

double DoubleRPIAccelY;
double DoubleRPIAccelY2 = 0;

double DoubleRPIAccelZ;
double DoubleRPIAccelZ2 = 0;

double DoubleRPIGyroX;
double DoubleRPIGyroX2 = 0;

double DoubleRPIGyroY;
double DoubleRPIGyroY2 = 0;

double DoubleRPIGyroZ;
double DoubleRPIGyroZ2 = 0;


//트레이닝 키트
bool BoolKitRedCheck;
bool BoolKitRedCheck2 = 0;

double DoubleKitMotorTemp;
double DoubleKitMotorTemp2 = 0;

bool BoolKitChromeCheck;
bool BoolKitChromeCheck2 = 0;

bool BoolKitBlackCheck;
bool BoolKitBlackCheck2 = 0;

bool BoolKitMentosCheck;
bool BoolKitMentosCheck2 = 0;

bool BoolKitHousingLocationCheck;
bool BoolKitHousingLocationCheck2 = 0;

bool BoolKitChromeSeperationCheck;
bool BoolKitChromeSeperationCheck2 = 0;

bool BoolKitRedSeperationCheck;
bool BoolKitRedSeperationCheck2 = 0;

bool BoolKitBlackSeperationCheck;
bool BoolKitBlackSeperationCheck2 = 0;

//RPI002
float FloatRPI2Gas;
float FloatRPI2Gas2 = 0;

uint32_t UIntRPI2Sound;
uint32_t UIntRPI2Sound2 = 0;

uint32_t UIntRPI2IR;
uint32_t UIntRPI2IR2 = 0;

float FloatRPI2Temp;
float FloatRPI2Temp2 = 0;

float FloatRPI2Humi;
float FloatRPI2Humi2 = 0;

double DoubleRPI2AccelX;
double DoubleRPI2AccelX2 = 0;

double DoubleRPI2AccelY;
double DoubleRPI2AccelY2 = 0;

double DoubleRPI2AccelZ;
double DoubleRPI2AccelZ2 = 0;

double DoubleRPI2GyroX;
double DoubleRPI2GyroX2 = 0;

double DoubleRPI2GyroY;
double DoubleRPI2GyroY2 = 0;

double DoubleRPI2GyroZ;
double DoubleRPI2GyroZ2 = 0;



static void RPITempValueTest(void);
static void RPIHumiValueTest(void);
static void RPIGasValueTest(void);
static void RPIIRValueTest(void);
static void RPISoundValueTest(void);
static void RPIAccelXValueTest(void);
static void RPIAccelYValueTest(void);
static void RPIAccelZValueTest(void);
static void RPIGyroXValueTest(void);
static void RPIGyroYValueTest(void);
static void RPIGyroZValueTest(void);

static void BoolKitRedCheckTest(void);
static void DoubleKitMotorTempTest(void);
static void BoolKitChromeCheckTest(void);
static void BoolKitBlackCheckTest(void);
static void BoolKitMentosCheckTest(void);
static void BoolKitHousingLocationCheckTest(void);
static void BoolKitChromeSeperationCheckTest(void);
static void BoolKitRedSeperationCheckTest(void);
static void BoolKitBlackSeperationCheckTest(void);

static void RPI2GasValueTest(void);
static void RPI2SoundValueTest(void);
static void RPI2IRValueTest(void);
static void RPI2TempValueTest(void);
static void RPI2HumiValueTest(void);
static void RPI2AccelXValueTest(void);
static void RPI2AccelYValueTest(void);
static void RPI2AccelZValueTest(void);
static void RPI2GyroXValueTest(void);
static void RPI2GyroYValueTest(void);
static void RPI2GyroZValueTest(void);

/***************/
/* ReaderGroup */
/***************/

UA_StatusCode
UA_Server_addReaderGroup(UA_Server *server, UA_NodeId connectionIdentifier,
                         const UA_ReaderGroupConfig *readerGroupConfig,
                         UA_NodeId *readerGroupIdentifier) {
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_ReaderGroupConfig tmpReaderGroupConfig;

    /* Check for valid readergroup configuration */
    if(!readerGroupConfig) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    /* Search the connection by the given connectionIdentifier */
    UA_PubSubConnection *currentConnectionContext =
        UA_PubSubConnection_findConnectionbyId(server, connectionIdentifier);
    if(!currentConnectionContext) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    /* Allocate memory for new reader group */
    UA_ReaderGroup *newGroup = (UA_ReaderGroup *)UA_calloc(1, sizeof(UA_ReaderGroup));
    if(!newGroup) {
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }

    /* Generate nodeid for the readergroup identifier */
    newGroup->linkedConnection = currentConnectionContext->identifier;
    UA_PubSubManager_generateUniqueNodeId(server, &newGroup->identifier);
    if(readerGroupIdentifier) {
        UA_NodeId_copy(&newGroup->identifier, readerGroupIdentifier);
    }

    /* Deep copy of the config */
    retval |= UA_ReaderGroupConfig_copy(readerGroupConfig, &tmpReaderGroupConfig);
    newGroup->config = tmpReaderGroupConfig;
    retval |= UA_ReaderGroup_addSubscribeCallback(server, newGroup);
    LIST_INSERT_HEAD(&currentConnectionContext->readerGroups, newGroup, listEntry);
    currentConnectionContext->readerGroupsSize++;

#ifdef UA_ENABLE_PUBSUB_INFORMATIONMODEL
    addReaderGroupRepresentation(server, newGroup);
#endif

    return retval;
}

UA_StatusCode
UA_Server_removeReaderGroup(UA_Server *server, UA_NodeId groupIdentifier) {
    UA_ReaderGroup* readerGroup = UA_ReaderGroup_findRGbyId(server, groupIdentifier);
    if(readerGroup == NULL) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    /* Search the connection to which the given readergroup is connected to */
    UA_PubSubConnection *connection =
        UA_PubSubConnection_findConnectionbyId(server, readerGroup->linkedConnection);
    if(connection == NULL) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    /* Unregister subscribe callback */
    UA_PubSubManager_removeRepeatedPubSubCallback(server, readerGroup->subscribeCallbackId);
#ifdef UA_ENABLE_PUBSUB_INFORMATIONMODEL
    /* To Do:RemoveGroupRepresentation(server, &readerGroup->identifier) */
#endif

    /* UA_Server_ReaderGroup_delete also removes itself from the list */
    UA_Server_ReaderGroup_delete(server, readerGroup);
    /* Remove readerGroup from Connection */
    LIST_REMOVE(readerGroup, listEntry);
    UA_free(readerGroup);
    return UA_STATUSCODE_GOOD;
}

/* TODO: Implement
UA_StatusCode
UA_Server_ReaderGroup_updateConfig(UA_Server *server, UA_NodeId readerGroupIdentifier,
                                   const UA_ReaderGroupConfig *config) {
    return UA_STATUSCODE_BADNOTIMPLEMENTED;
}
*/

UA_StatusCode
UA_Server_ReaderGroup_getConfig(UA_Server *server, UA_NodeId readerGroupIdentifier,
                                UA_ReaderGroupConfig *config) {
    if(!config) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    /* Identify the readergroup through the readerGroupIdentifier */
    UA_ReaderGroup *currentReaderGroup = UA_ReaderGroup_findRGbyId(server, readerGroupIdentifier);
    if(!currentReaderGroup) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    UA_ReaderGroupConfig tmpReaderGroupConfig;
    /* deep copy of the actual config */
    UA_ReaderGroupConfig_copy(&currentReaderGroup->config, &tmpReaderGroupConfig);
    *config = tmpReaderGroupConfig;
    return UA_STATUSCODE_GOOD;
}

void
UA_Server_ReaderGroup_delete(UA_Server* server, UA_ReaderGroup *readerGroup) {
    /* To Do Call UA_ReaderGroupConfig_delete */
    UA_DataSetReader *dataSetReader, *tmpDataSetReader;
    LIST_FOREACH_SAFE(dataSetReader, &readerGroup->readers, listEntry, tmpDataSetReader) {
        UA_DataSetReader_delete(server, dataSetReader);
    }
    UA_PubSubConnection* pConn =
        UA_PubSubConnection_findConnectionbyId(server, readerGroup->linkedConnection);
    if(pConn != NULL) {
        pConn->readerGroupsSize--;
    }

    /* Delete ReaderGroup and its members */
    UA_String_deleteMembers(&readerGroup->config.name);
    UA_NodeId_deleteMembers(&readerGroup->linkedConnection);
    UA_NodeId_deleteMembers(&readerGroup->identifier);
}

UA_StatusCode
UA_ReaderGroupConfig_copy(const UA_ReaderGroupConfig *src,
                          UA_ReaderGroupConfig *dst) {
    /* Currently simple memcpy only */
    memcpy(&dst->securityParameters, &src->securityParameters, sizeof(UA_PubSubSecurityParameters));
    UA_String_copy(&src->name, &dst->name);
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
checkReaderIdentifier(UA_Server *server, UA_NetworkMessage *pMsg, UA_DataSetReader *reader) {
    if(!pMsg->groupHeaderEnabled &&
       !pMsg->groupHeader.writerGroupIdEnabled &&
       !pMsg->payloadHeaderEnabled) {
        UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER,
                    "Cannot process DataSetReader without WriterGroup"
                    "and DataSetWriter identifiers");
        return UA_STATUSCODE_BADNOTIMPLEMENTED;
    }

    if((reader->config.writerGroupId == pMsg->groupHeader.writerGroupId) &&
       (reader->config.dataSetWriterId == *pMsg->payloadHeader.dataSetPayloadHeader.dataSetWriterIds)) {
        UA_LOG_DEBUG(&server->config.logger, UA_LOGCATEGORY_SERVER,
                     "DataSetReader found. Process NetworkMessage");
        return UA_STATUSCODE_GOOD;
    }

    return UA_STATUSCODE_BADNOTFOUND;
}

static UA_StatusCode
getReaderFromIdentifier(UA_Server *server, UA_NetworkMessage *pMsg,
                        UA_DataSetReader **dataSetReader, UA_PubSubConnection *pConnection) {
    UA_StatusCode retval = UA_STATUSCODE_BADNOTFOUND;
    if(!pMsg->publisherIdEnabled) {
        UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER,
                    "Cannot process DataSetReader without PublisherId");
        return UA_STATUSCODE_BADNOTIMPLEMENTED; /* TODO: Handle DSR without PublisherId */
    }

    UA_ReaderGroup* readerGroup;
    LIST_FOREACH(readerGroup, &pConnection->readerGroups, listEntry) {
        UA_DataSetReader *tmpReader;
        LIST_FOREACH(tmpReader, &readerGroup->readers, listEntry) {
            switch (pMsg->publisherIdType) {
            case UA_PUBLISHERDATATYPE_BYTE:
                if(tmpReader->config.publisherId.type == &UA_TYPES[UA_TYPES_BYTE] &&
                   pMsg->publisherIdType == UA_PUBLISHERDATATYPE_BYTE &&
                   pMsg->publisherId.publisherIdByte == *(UA_Byte*)tmpReader->config.publisherId.data) {
                    retval = checkReaderIdentifier(server, pMsg, tmpReader);
                }
                break;
            case UA_PUBLISHERDATATYPE_UINT16:
                if(tmpReader->config.publisherId.type == &UA_TYPES[UA_TYPES_UINT16] &&
                   pMsg->publisherIdType == UA_PUBLISHERDATATYPE_UINT16 &&
                   pMsg->publisherId.publisherIdUInt16 == *(UA_UInt16*) tmpReader->config.publisherId.data) {
                    retval = checkReaderIdentifier(server, pMsg, tmpReader);
                }
                break;
            case UA_PUBLISHERDATATYPE_UINT32:
                if(tmpReader->config.publisherId.type == &UA_TYPES[UA_TYPES_UINT32] &&
                   pMsg->publisherIdType == UA_PUBLISHERDATATYPE_UINT32 &&
                   pMsg->publisherId.publisherIdUInt32 == *(UA_UInt32*)tmpReader->config.publisherId.data) {
                    retval = checkReaderIdentifier(server, pMsg, tmpReader);
                }
                break;
            case UA_PUBLISHERDATATYPE_UINT64:
                if(tmpReader->config.publisherId.type == &UA_TYPES[UA_TYPES_UINT64] &&
                   pMsg->publisherIdType == UA_PUBLISHERDATATYPE_UINT64 &&
                   pMsg->publisherId.publisherIdUInt64 == *(UA_UInt64*)tmpReader->config.publisherId.data) {
                    retval = checkReaderIdentifier(server, pMsg, tmpReader);
                }
                break;
            case UA_PUBLISHERDATATYPE_STRING:
                if(tmpReader->config.publisherId.type == &UA_TYPES[UA_TYPES_STRING] &&
                   pMsg->publisherIdType == UA_PUBLISHERDATATYPE_STRING &&
                   UA_String_equal(&pMsg->publisherId.publisherIdString,
                                   (UA_String*)tmpReader->config.publisherId.data)) {
                    retval = checkReaderIdentifier(server, pMsg, tmpReader);
                }
                break;
            default:
                return UA_STATUSCODE_BADINTERNALERROR;
            }

            if(retval == UA_STATUSCODE_GOOD) {
                *dataSetReader = tmpReader;
                return UA_STATUSCODE_GOOD;
            }
        }
    }

    // UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER,
    //             "Dataset reader not found. Check PublisherID, WriterGroupID and DatasetWriterID");
    return UA_STATUSCODE_BADNOTFOUND;
}

UA_ReaderGroup *
UA_ReaderGroup_findRGbyId(UA_Server *server, UA_NodeId identifier) {
    UA_PubSubConnection *pubSubConnection;
    TAILQ_FOREACH(pubSubConnection, &server->pubSubManager.connections, listEntry){
        UA_ReaderGroup* readerGroup = NULL;
        LIST_FOREACH(readerGroup, &pubSubConnection->readerGroups, listEntry) {
            if(UA_NodeId_equal(&identifier, &readerGroup->identifier)) {
                return readerGroup;
            }

        }
    }
    return NULL;
}

UA_DataSetReader *UA_ReaderGroup_findDSRbyId(UA_Server *server, UA_NodeId identifier) {
    UA_PubSubConnection *pubSubConnection;
    TAILQ_FOREACH(pubSubConnection, &server->pubSubManager.connections, listEntry){
        UA_ReaderGroup* readerGroup = NULL;
        LIST_FOREACH(readerGroup, &pubSubConnection->readerGroups, listEntry) {
            UA_DataSetReader *tmpReader;
            LIST_FOREACH(tmpReader, &readerGroup->readers, listEntry) {
                if(UA_NodeId_equal(&tmpReader->identifier, &identifier)) {
                    return tmpReader;
                }
            }
        }
    }
    return NULL;
}

/* This callback triggers the collection and reception of NetworkMessages and the
 * contained DataSetMessages. */
void UA_ReaderGroup_subscribeCallback(UA_Server *server, UA_ReaderGroup *readerGroup) {
    UA_PubSubConnection *connection =
        UA_PubSubConnection_findConnectionbyId(server, readerGroup->linkedConnection);
    UA_ByteString buffer;
    if(UA_ByteString_allocBuffer(&buffer, 512) != UA_STATUSCODE_GOOD) {
        UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER, "Message buffer alloc failed!");
        return;
    }
    /* Receive the message. Blocks for 100ms */
    connection->channel->receive(connection->channel, &buffer, NULL, 1000);
    if(buffer.length > 0) {
        //UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_USERLAND, "Message received:");
        UA_NetworkMessage currentNetworkMessage;
        memset(&currentNetworkMessage, 0, sizeof(UA_NetworkMessage));
        size_t currentPosition = 0;
        UA_NetworkMessage_decodeBinary(&buffer, &currentPosition, &currentNetworkMessage);
        UA_Server_processNetworkMessage(server, &currentNetworkMessage, connection);

        UA_Variant *p1 = (UA_Variant*)malloc(sizeof(UA_NetworkMessage));

        if(p1->data != NULL)
        {
            //UA_Variant 변수설정
            UA_Variant VariantRPITemperature;
            UA_Variant VariantRPIHumidity;
            UA_Variant VariantRPIGas;
            UA_Variant VariantRPIIR;
            UA_Variant VariantRPISound;
            UA_Variant VariantRPIAccelX;
            UA_Variant VariantRPIAccelY;
            UA_Variant VariantRPIAccelZ;
            UA_Variant VariantRPIGyroX;
            UA_Variant VariantRPIGyroY;
            UA_Variant VariantRPIGyroZ;

            UA_Variant VariantKitRedCheck;
            UA_Variant VariantKitMotorTemp;
            UA_Variant VariantKitChromeCheck;
            UA_Variant VariantKitBlackCheck;
            UA_Variant VariantKitMentosCheck;
            UA_Variant VariantKitHousingLocationCheck;
            UA_Variant VariantKitChromeSeperationCheck;
            UA_Variant VariantKitRedSeperationCheck;
            UA_Variant VariantKitBlackSeperationCheck;

            UA_Variant VariantRPI2Gas;
            UA_Variant VariantRPI2Sound;
            UA_Variant VariantRPI2IR;
            UA_Variant VariantRPI2Temperature;
            UA_Variant VariantRPI2Humidity;
            UA_Variant VariantRPI2AccelX;
            UA_Variant VariantRPI2AccelY;
            UA_Variant VariantRPI2AccelZ;
            UA_Variant VariantRPI2GyroX;
            UA_Variant VariantRPI2GyroY;
            UA_Variant VariantRPI2GyroZ;

            //변수 초기화
            UA_Variant_init(&VariantRPITemperature);
            UA_Variant_init(&VariantRPIHumidity);
            UA_Variant_init(&VariantRPIGas);
            UA_Variant_init(&VariantRPIIR);
            UA_Variant_init(&VariantRPISound);
            UA_Variant_init(&VariantRPIAccelX);
            UA_Variant_init(&VariantRPIAccelY);
            UA_Variant_init(&VariantRPIAccelZ);
            UA_Variant_init(&VariantRPIGyroX);
            UA_Variant_init(&VariantRPIGyroY);
            UA_Variant_init(&VariantRPIGyroZ);

            UA_Variant_init(&VariantKitRedCheck);
            UA_Variant_init(&VariantKitMotorTemp);
            UA_Variant_init(&VariantKitChromeCheck);
            UA_Variant_init(&VariantKitBlackCheck);
            UA_Variant_init(&VariantKitMentosCheck);
            UA_Variant_init(&VariantKitHousingLocationCheck);
            UA_Variant_init(&VariantKitChromeSeperationCheck);
            UA_Variant_init(&VariantKitRedSeperationCheck);
            UA_Variant_init(&VariantKitBlackSeperationCheck);

            UA_Variant_init(&VariantRPI2Gas);
            UA_Variant_init(&VariantRPI2Sound);
            UA_Variant_init(&VariantRPI2IR);
            UA_Variant_init(&VariantRPI2Temperature);
            UA_Variant_init(&VariantRPI2Humidity);
            UA_Variant_init(&VariantRPI2AccelX);
            UA_Variant_init(&VariantRPI2AccelY);
            UA_Variant_init(&VariantRPI2AccelZ);
            UA_Variant_init(&VariantRPI2GyroX);
            UA_Variant_init(&VariantRPI2GyroY);
            UA_Variant_init(&VariantRPI2GyroZ);

            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50377), &VariantRPITemperature);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50376), &VariantRPIHumidity);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50378), &VariantRPIGas);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50379), &VariantRPIIR);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50380), &VariantRPISound);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50381), &VariantRPIAccelX);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50382), &VariantRPIAccelY);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50383), &VariantRPIAccelZ);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50384), &VariantRPIGyroX);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 51022), &VariantRPIGyroY);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50386), &VariantRPIGyroZ);

            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50400), &VariantKitRedCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50401), &VariantKitMotorTemp);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50402), &VariantKitChromeCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50403), &VariantKitBlackCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50404), &VariantKitMentosCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50405), &VariantKitHousingLocationCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50406), &VariantKitChromeSeperationCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50407), &VariantKitRedSeperationCheck);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50408), &VariantKitBlackSeperationCheck);

            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50422), &VariantRPI2Gas);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50423), &VariantRPI2Sound);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50424), &VariantRPI2IR);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50425), &VariantRPI2Temperature);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50426), &VariantRPI2Humidity);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50427), &VariantRPI2AccelX);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50428), &VariantRPI2AccelY);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50429), &VariantRPI2AccelZ);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50430), &VariantRPI2GyroX);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50431), &VariantRPI2GyroY);
            UA_Server_readValue(server, UA_NODEID_NUMERIC(0, 50432), &VariantRPI2GyroZ);

            FloatRPITemp2 = FloatRPITemp;
            FloatRPIHumi2 = FloatRPIHumi;
            FloatRPIGas2 = FloatRPIGas;
            UIntRPIIR2 = UIntRPIIR;
            UIntRPISound2 = UIntRPISound;
            DoubleRPIAccelX2 = DoubleRPIAccelX;
            DoubleRPIAccelY2 = DoubleRPIAccelY;
            DoubleRPIAccelZ2 = DoubleRPIAccelZ;
            DoubleRPIGyroX2 = DoubleRPIGyroX;
            DoubleRPIGyroY2 = DoubleRPIGyroY;
            DoubleRPIGyroZ2 = DoubleRPIGyroZ;

            BoolKitRedCheck2 = BoolKitRedCheck;
            DoubleKitMotorTemp2 = DoubleKitMotorTemp;
            BoolKitChromeCheck2 = BoolKitChromeCheck;
            BoolKitBlackCheck2 = BoolKitBlackCheck;
            BoolKitMentosCheck2 = BoolKitMentosCheck;
            BoolKitHousingLocationCheck2 = BoolKitHousingLocationCheck;
            BoolKitChromeSeperationCheck2 = BoolKitChromeSeperationCheck;
            BoolKitRedSeperationCheck2 = BoolKitRedSeperationCheck;
            BoolKitBlackSeperationCheck2 = BoolKitBlackSeperationCheck;

            FloatRPI2Gas2 = FloatRPI2Gas;
            UIntRPI2Sound2 = UIntRPI2Sound;
            UIntRPI2IR2 = UIntRPI2IR;
            FloatRPI2Temp2 = FloatRPI2Temp;
            FloatRPI2Humi2 = FloatRPI2Humi;           
            DoubleRPI2AccelX2 = DoubleRPI2AccelX;
            DoubleRPI2AccelY2 = DoubleRPI2AccelY;
            DoubleRPI2AccelZ2 = DoubleRPI2AccelZ;
            DoubleRPI2GyroX2 = DoubleRPI2GyroX;
            DoubleRPI2GyroY2 = DoubleRPI2GyroY;
            DoubleRPI2GyroZ2 = DoubleRPI2GyroZ;


            FloatRPITemp = *(UA_Float*)VariantRPITemperature.data;
            FloatRPIHumi = *(UA_Float*)VariantRPIHumidity.data;
            FloatRPIGas = *(UA_Float*)VariantRPIGas.data;
            UIntRPIIR= *(UA_UInt32*)VariantRPIIR.data;
            UIntRPISound = *(UA_UInt32*)VariantRPISound.data;
            DoubleRPIAccelX = *(UA_Double*)VariantRPIAccelX.data;
            DoubleRPIAccelY = *(UA_Double*)VariantRPIAccelY.data;
            DoubleRPIAccelZ = *(UA_Double*)VariantRPIAccelZ.data;
            DoubleRPIGyroX = *(UA_Double*)VariantRPIGyroX.data;
            DoubleRPIGyroY = *(UA_Double*)VariantRPIGyroY.data;
            DoubleRPIGyroZ = *(UA_Double*)VariantRPIGyroZ.data;

            BoolKitRedCheck = *(UA_Boolean*)VariantKitRedCheck.data;
            DoubleKitMotorTemp= *(UA_Double*)VariantKitMotorTemp.data;
            BoolKitChromeCheck = *(UA_Boolean*)VariantKitChromeCheck.data;
            BoolKitBlackCheck = *(UA_Boolean*)VariantKitBlackCheck.data;
            BoolKitMentosCheck = *(UA_Boolean*)VariantKitMentosCheck.data;
            BoolKitHousingLocationCheck = *(UA_Boolean*)VariantKitHousingLocationCheck.data;
            BoolKitChromeSeperationCheck = *(UA_Boolean*)VariantKitChromeSeperationCheck.data;
            BoolKitRedSeperationCheck = *(UA_Boolean*)VariantKitRedSeperationCheck.data;
            BoolKitBlackSeperationCheck = *(UA_Boolean*)VariantKitBlackSeperationCheck.data;

            FloatRPI2Gas = *(UA_Float*)VariantRPI2Gas.data;
            UIntRPI2Sound = *(UA_UInt32*)VariantRPI2Sound.data;
            UIntRPI2IR = *(UA_UInt32*)VariantRPI2IR.data;
            FloatRPI2Temp = *(UA_Float*)VariantRPI2Temperature.data;
            FloatRPI2Humi = *(UA_Float*)VariantRPI2Humidity.data;
            DoubleRPI2AccelX = *(UA_Double*)VariantRPI2AccelX.data;
            DoubleRPI2AccelY = *(UA_Double*)VariantRPI2AccelY.data;
            DoubleRPI2AccelZ = *(UA_Double*)VariantRPI2AccelZ.data;
            DoubleRPI2GyroX = *(UA_Double*)VariantRPI2GyroX.data;
            DoubleRPI2GyroY = *(UA_Double*)VariantRPI2GyroY.data;
            DoubleRPI2GyroZ = *(UA_Double*)VariantRPI2GyroZ.data;


            RPITempValueTest();
            RPIHumiValueTest();
            RPIGasValueTest();
            RPIIRValueTest();
            RPISoundValueTest();
            RPIAccelXValueTest();
            RPIAccelYValueTest();
            RPIAccelZValueTest();
            RPIGyroXValueTest();
            RPIGyroYValueTest();
            RPIGyroZValueTest();

            BoolKitRedCheckTest();
            DoubleKitMotorTempTest();
            BoolKitChromeCheckTest();
            BoolKitBlackCheckTest();
            BoolKitMentosCheckTest();
            BoolKitHousingLocationCheckTest();
            BoolKitChromeSeperationCheckTest();
            BoolKitRedSeperationCheckTest();
            BoolKitBlackSeperationCheckTest();

            RPI2GasValueTest();
            RPI2SoundValueTest();
            RPI2IRValueTest();
            RPI2TempValueTest();
            RPI2HumiValueTest();
            RPI2AccelXValueTest();
            RPI2AccelYValueTest();
            RPI2AccelZValueTest();
            RPI2GyroXValueTest();
            RPI2GyroYValueTest();
            RPI2GyroZValueTest();

            UA_Variant_deleteMembers(&VariantRPITemperature);
            UA_Variant_deleteMembers(&VariantRPIHumidity);
            UA_Variant_deleteMembers(&VariantRPIGas);
            UA_Variant_deleteMembers(&VariantRPIIR);
            UA_Variant_deleteMembers(&VariantRPISound);
            UA_Variant_deleteMembers(&VariantRPIAccelX);
            UA_Variant_deleteMembers(&VariantRPIAccelY);
            UA_Variant_deleteMembers(&VariantRPIAccelZ);
            UA_Variant_deleteMembers(&VariantRPIGyroX);
            UA_Variant_deleteMembers(&VariantRPIGyroY);
            UA_Variant_deleteMembers(&VariantRPIGyroZ);

            UA_Variant_deleteMembers(&VariantKitRedCheck);
            UA_Variant_deleteMembers(&VariantKitMotorTemp);
            UA_Variant_deleteMembers(&VariantKitChromeCheck);
            UA_Variant_deleteMembers(&VariantKitBlackCheck);
            UA_Variant_deleteMembers(&VariantKitMentosCheck);
            UA_Variant_deleteMembers(&VariantKitHousingLocationCheck);
            UA_Variant_deleteMembers(&VariantKitChromeSeperationCheck);
            UA_Variant_deleteMembers(&VariantKitRedSeperationCheck);
            UA_Variant_deleteMembers(&VariantKitBlackSeperationCheck);

            UA_Variant_deleteMembers(&VariantRPI2Gas);
            UA_Variant_deleteMembers(&VariantRPI2Sound);
            UA_Variant_deleteMembers(&VariantRPI2IR);
            UA_Variant_deleteMembers(&VariantRPI2Temperature);
            UA_Variant_deleteMembers(&VariantRPI2Humidity);
            UA_Variant_deleteMembers(&VariantRPI2AccelX);
            UA_Variant_deleteMembers(&VariantRPI2AccelY);
            UA_Variant_deleteMembers(&VariantRPI2AccelZ);
            UA_Variant_deleteMembers(&VariantRPI2GyroX);
            UA_Variant_deleteMembers(&VariantRPI2GyroY);
            UA_Variant_deleteMembers(&VariantRPI2GyroZ);

        }
        free(p1);

        UA_NetworkMessage_deleteMembers(&currentNetworkMessage);
    }

    UA_ByteString_deleteMembers(&buffer);
}

/* Add new subscribeCallback. The first execution is triggered directly after
 * creation. */
UA_StatusCode
UA_ReaderGroup_addSubscribeCallback(UA_Server *server, UA_ReaderGroup *readerGroup) {
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    retval |= UA_PubSubManager_addRepeatedCallback(server,
                                                   (UA_ServerCallback) UA_ReaderGroup_subscribeCallback,
                                                   readerGroup, 5, &readerGroup->subscribeCallbackId);

    if(retval == UA_STATUSCODE_GOOD) {
        readerGroup->subscribeCallbackIsRegistered = true;
    }

    /* Run once after creation */
    UA_ReaderGroup_subscribeCallback(server, readerGroup);
    return retval;
}

/**********/
/* Reader */
/**********/

UA_StatusCode
UA_Server_addDataSetReader(UA_Server *server, UA_NodeId readerGroupIdentifier,
                                      const UA_DataSetReaderConfig *dataSetReaderConfig,
                                      UA_NodeId *readerIdentifier) {
    /* Search the reader group by the given readerGroupIdentifier */
    UA_ReaderGroup *readerGroup = UA_ReaderGroup_findRGbyId(server, readerGroupIdentifier);

    if(!dataSetReaderConfig) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    if(readerGroup == NULL) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    /* Allocate memory for new DataSetReader */
    UA_DataSetReader *newDataSetReader = (UA_DataSetReader *)UA_calloc(1, sizeof(UA_DataSetReader));
    /* Copy the config into the new dataSetReader */
    UA_DataSetReaderConfig_copy(dataSetReaderConfig, &newDataSetReader->config);
    newDataSetReader->linkedReaderGroup = readerGroup->identifier;
    UA_PubSubManager_generateUniqueNodeId(server, &newDataSetReader->identifier);
    if(readerIdentifier != NULL) {
        UA_NodeId_copy(&newDataSetReader->identifier, readerIdentifier);
    }

    /* Add the new reader to the group */
    LIST_INSERT_HEAD(&readerGroup->readers, newDataSetReader, listEntry);
    readerGroup->readersCount++;

#ifdef UA_ENABLE_PUBSUB_INFORMATIONMODEL
    addDataSetReaderRepresentation(server, newDataSetReader);
#endif

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
UA_Server_removeDataSetReader(UA_Server *server, UA_NodeId readerIdentifier) {
    /* Remove datasetreader given by the identifier */
    UA_DataSetReader *dataSetReader = UA_ReaderGroup_findDSRbyId(server, readerIdentifier);
    if(!dataSetReader) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

#ifdef UA_ENABLE_PUBSUB_INFORMATIONMODEL
    removeDataSetReaderRepresentation(server, dataSetReader);
#endif

    UA_DataSetReader_delete(server, dataSetReader);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
UA_Server_DataSetReader_updateConfig(UA_Server *server, UA_NodeId dataSetReaderIdentifier,
                                     UA_NodeId readerGroupIdentifier,
                                     const UA_DataSetReaderConfig *config) {
    if(config == NULL) {
       return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    UA_DataSetReader *currentDataSetReader =
        UA_ReaderGroup_findDSRbyId(server, dataSetReaderIdentifier);
    UA_ReaderGroup *currentReaderGroup =
        UA_ReaderGroup_findRGbyId(server, readerGroupIdentifier);
    if(!currentDataSetReader) {
       return UA_STATUSCODE_BADNOTFOUND;
    }

    /* The update functionality will be extended during the next PubSub batches.
     * Currently is only a change of the publishing interval possible. */
    if(currentDataSetReader->config.writerGroupId != config->writerGroupId) {
       UA_PubSubManager_removeRepeatedPubSubCallback(server, currentReaderGroup->subscribeCallbackId);
       currentDataSetReader->config.writerGroupId = config->writerGroupId;
       UA_ReaderGroup_subscribeCallback(server, currentReaderGroup);
    }
    else {
        UA_LOG_WARNING(&server->config.logger, UA_LOGCATEGORY_SERVER,
                       "No or unsupported ReaderGroup update.");
    }

    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
UA_Server_DataSetReader_getConfig(UA_Server *server, UA_NodeId dataSetReaderIdentifier,
                                 UA_DataSetReaderConfig *config) {
    if(!config) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    UA_DataSetReader *currentDataSetReader =
        UA_ReaderGroup_findDSRbyId(server, dataSetReaderIdentifier);
    if(!currentDataSetReader) {
        return UA_STATUSCODE_BADNOTFOUND;
    }

    UA_DataSetReaderConfig tmpReaderConfig;
    /* Deep copy of the actual config */
    UA_DataSetReaderConfig_copy(&currentDataSetReader->config, &tmpReaderConfig);
    *config = tmpReaderConfig;
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode
UA_DataSetReaderConfig_copy(const UA_DataSetReaderConfig *src,
                            UA_DataSetReaderConfig *dst) {
    memset(dst, 0, sizeof(UA_DataSetReaderConfig));
    UA_StatusCode retVal = UA_String_copy(&src->name, &dst->name);
    if(retVal != UA_STATUSCODE_GOOD) {
        return retVal;
    }

    retVal = UA_Variant_copy(&src->publisherId, &dst->publisherId);
    if(retVal != UA_STATUSCODE_GOOD) {
        return retVal;
    }

    dst->writerGroupId = src->writerGroupId;
    dst->dataSetWriterId = src->dataSetWriterId;
    retVal = UA_DataSetMetaDataType_copy(&src->dataSetMetaData, &dst->dataSetMetaData);
    if(retVal != UA_STATUSCODE_GOOD) {
        return retVal;
    }

    dst->dataSetFieldContentMask = src->dataSetFieldContentMask;
    dst->messageReceiveTimeout = src->messageReceiveTimeout;

    /* Currently memcpy is used to copy the securityParameters */
    memcpy(&dst->securityParameters, &src->securityParameters, sizeof(UA_PubSubSecurityParameters));
    retVal = UA_UadpDataSetReaderMessageDataType_copy(&src->messageSettings, &dst->messageSettings);
    if(retVal != UA_STATUSCODE_GOOD) {
        return retVal;
    }

    return UA_STATUSCODE_GOOD;
}

/* This Method is used to initially set the SubscribedDataSet to
 * TargetVariablesType and to create the list of target Variables of a
 * SubscribedDataSetType. */
UA_StatusCode
UA_Server_DataSetReader_createTargetVariables(UA_Server *server,
                                              UA_NodeId dataSetReaderIdentifier,
                                              UA_TargetVariablesDataType *targetVariables) {
    UA_StatusCode retval = UA_STATUSCODE_BADUNEXPECTEDERROR;
    UA_DataSetReader* pDS = UA_ReaderGroup_findDSRbyId(server, dataSetReaderIdentifier);
    if(pDS == NULL) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    if(pDS->subscribedDataSetTarget.targetVariablesSize > 0) {
        UA_TargetVariablesDataType_deleteMembers(&pDS->subscribedDataSetTarget);
        pDS->subscribedDataSetTarget.targetVariablesSize = 0;
        pDS->subscribedDataSetTarget.targetVariables = NULL;
    }

    /* Set subscribed dataset to TargetVariableType */
    pDS->subscribedDataSetType = UA_PUBSUB_SDS_TARGET;
    retval = UA_TargetVariablesDataType_copy(targetVariables, &pDS->subscribedDataSetTarget);
    return retval;
}

/* Adds Subscribed Variables from the DataSetMetaData for the given DataSet into
 * the given parent node and creates the corresponding data in the
 * targetVariables of the DataSetReader */
UA_StatusCode
UA_Server_DataSetReader_addTargetVariables(UA_Server *server, UA_NodeId *parentNode,
                                           UA_NodeId dataSetReaderIdentifier,
                                           UA_SubscribedDataSetEnumType sdsType) {
    if((server == NULL) || (parentNode == NULL)) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_DataSetReader* pDataSetReader = UA_ReaderGroup_findDSRbyId(server, dataSetReaderIdentifier);
    if(pDataSetReader == NULL) {
        return UA_STATUSCODE_BADINVALIDARGUMENT;
    }

    UA_TargetVariablesDataType targetVars;
    targetVars.targetVariablesSize = pDataSetReader->config.dataSetMetaData.fieldsSize;
    targetVars.targetVariables = (UA_FieldTargetDataType*)
        UA_calloc(targetVars.targetVariablesSize, sizeof(UA_FieldTargetDataType));

    for (size_t i = 0; i < pDataSetReader->config.dataSetMetaData.fieldsSize; i++) {
        UA_VariableAttributes vAttr = UA_VariableAttributes_default;
        vAttr.valueRank = pDataSetReader->config.dataSetMetaData.fields[i].valueRank;
        if(pDataSetReader->config.dataSetMetaData.fields[i].arrayDimensionsSize > 0) {
            retval = UA_Array_copy(pDataSetReader->config.dataSetMetaData.fields[i].arrayDimensions,
                                   pDataSetReader->config.dataSetMetaData.fields[i].arrayDimensionsSize,
                                   (void**)&vAttr.arrayDimensions, &UA_TYPES[UA_TYPES_UINT32]);
            if(retval == UA_STATUSCODE_GOOD) {
                vAttr.arrayDimensionsSize =
                    pDataSetReader->config.dataSetMetaData.fields[i].arrayDimensionsSize;
            }

        }

        vAttr.dataType = pDataSetReader->config.dataSetMetaData.fields[i].dataType;

        vAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
        UA_LocalizedText_copy(&pDataSetReader->config.dataSetMetaData.fields[i].description,
                              &vAttr.description);
        UA_QualifiedName qn;
        UA_QualifiedName_init(&qn);
        char szTmpName[UA_MAX_SIZENAME];
        if(pDataSetReader->config.dataSetMetaData.fields[i].name.length > 0) {
            UA_UInt16 slen = UA_MAX_SIZENAME -1;
            vAttr.displayName.locale = UA_STRING("en-US");
            vAttr.displayName.text = pDataSetReader->config.dataSetMetaData.fields[i].name;
            if(pDataSetReader->config.dataSetMetaData.fields[i].name.length < slen) {
                slen = (UA_UInt16)pDataSetReader->config.dataSetMetaData.fields[i].name.length;
                UA_snprintf(szTmpName, sizeof(szTmpName), "%.*s", (int)slen,
                            (const char*)pDataSetReader->config.dataSetMetaData.fields[i].name.data);
            }

            szTmpName[slen] = '\0';
            qn = UA_QUALIFIEDNAME(1, szTmpName);
        }
        else {
            strcpy(szTmpName, "SubscribedVariable");
            vAttr.displayName = UA_LOCALIZEDTEXT("en-US", szTmpName);
            qn = UA_QUALIFIEDNAME(1, "SubscribedVariable");
        }

        /* Add variable to the given parent node */
        UA_NodeId newNode;
        retval = UA_Server_addVariableNode(server, UA_NODEID_NULL, *parentNode,
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), qn,
                                           UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                           vAttr, NULL, &newNode);
        if(retval == UA_STATUSCODE_GOOD) {
            UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_USERLAND,
                        "addVariableNode %s succeeded", szTmpName);
        }
        else {
            UA_LOG_ERROR(&server->config.logger, UA_LOGCATEGORY_USERLAND,
                         "addVariableNode: error 0x%x", retval);
        }

        UA_FieldTargetDataType_init(&targetVars.targetVariables[i]);
        targetVars.targetVariables[i].attributeId = UA_ATTRIBUTEID_VALUE;
        UA_NodeId_copy(&newNode, &targetVars.targetVariables[i].targetNodeId);
        UA_NodeId_deleteMembers(&newNode);
        if(vAttr.arrayDimensionsSize > 0) {
            UA_Array_delete(vAttr.arrayDimensions, vAttr.arrayDimensionsSize,
                            &UA_TYPES[UA_TYPES_UINT32]);
        }
    }

    if(sdsType == UA_PUBSUB_SDS_TARGET) {
        retval = UA_Server_DataSetReader_createTargetVariables(server, pDataSetReader->identifier,
                                                               &targetVars);
    }

    UA_TargetVariablesDataType_deleteMembers(&targetVars);
    return retval;
}

void
UA_Server_DataSetReader_process(UA_Server *server, UA_DataSetReader *dataSetReader,
                                UA_DataSetMessage* dataSetMsg) {
    if((dataSetReader == NULL) || (dataSetMsg == NULL) || (server == NULL)) {
        return;
    }

    if(!dataSetMsg->header.dataSetMessageValid) {
        UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER,
                    "DataSetMessage is discarded: message is not valid");
         /* To Do check ConfigurationVersion*/
         /*if(dataSetMsg->header.configVersionMajorVersionEnabled)
         * {
         * if(dataSetMsg->header.configVersionMajorVersion != dataSetReader->config.dataSetMetaData.configurationVersion.majorVersion)
         * {
         * UA_LOG_WARNING(server->config.logger, UA_LOGCATEGORY_SERVER, "DataSetMessage is discarded: ConfigurationVersion MajorVersion does not match");
         * return;
         * }
         } */
        return;
    }

    if(dataSetMsg->header.dataSetMessageType == UA_DATASETMESSAGE_DATAKEYFRAME) {
        if(dataSetMsg->header.fieldEncoding != UA_FIELDENCODING_RAWDATA) {
            size_t anzFields = dataSetMsg->data.keyFrameData.fieldCount;
            if(dataSetReader->config.dataSetMetaData.fieldsSize < anzFields) {
                anzFields = dataSetReader->config.dataSetMetaData.fieldsSize;
            }

            if(dataSetReader->subscribedDataSetTarget.targetVariablesSize < anzFields) {
                anzFields = dataSetReader->subscribedDataSetTarget.targetVariablesSize;
            }

            UA_StatusCode retVal = UA_STATUSCODE_GOOD;
            for(UA_UInt16 i = 0; i < anzFields; i++) {
                if(dataSetMsg->data.keyFrameData.dataSetFields[i].hasValue) {
                    if(dataSetReader->subscribedDataSetTarget.targetVariables[i].attributeId == UA_ATTRIBUTEID_VALUE) {
                        retVal = UA_Server_writeValue(server, dataSetReader->subscribedDataSetTarget.targetVariables[i].targetNodeId, dataSetMsg->data.keyFrameData.dataSetFields[i].value);
                        if(retVal != UA_STATUSCODE_GOOD) {
                            UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER, "Error Write Value KF %u: 0x%x", i, retVal);
                        }

                    }
                    else {
                        UA_WriteValue writeVal;
                        UA_WriteValue_init(&writeVal);
                        writeVal.attributeId = dataSetReader->subscribedDataSetTarget.targetVariables[i].attributeId;
                        writeVal.indexRange = dataSetReader->subscribedDataSetTarget.targetVariables[i].receiverIndexRange;
                        writeVal.nodeId = dataSetReader->subscribedDataSetTarget.targetVariables[i].targetNodeId;
                        UA_DataValue_copy(&dataSetMsg->data.keyFrameData.dataSetFields[i], &writeVal.value);
                        retVal = UA_Server_write(server, &writeVal);
                        if(retVal != UA_STATUSCODE_GOOD) {
                            UA_LOG_INFO(&server->config.logger, UA_LOGCATEGORY_SERVER, "Error Write KF %u: 0x%x", i, retVal);
                        }

                    }

                }

            }

        }

    }
}

void UA_DataSetReader_delete(UA_Server *server, UA_DataSetReader *dataSetReader) {
    /* Delete DataSetReader config */
    UA_String_deleteMembers(&dataSetReader->config.name);
    UA_Variant_deleteMembers(&dataSetReader->config.publisherId);
    UA_DataSetMetaDataType_deleteMembers(&dataSetReader->config.dataSetMetaData);
    UA_UadpDataSetReaderMessageDataType_deleteMembers(&dataSetReader->config.messageSettings);
    UA_TargetVariablesDataType_deleteMembers(&dataSetReader->subscribedDataSetTarget);

    /* Delete DataSetReader */
    UA_ReaderGroup* pGroup = UA_ReaderGroup_findRGbyId(server, dataSetReader->linkedReaderGroup);
    if(pGroup != NULL) {
        pGroup->readersCount--;
    }

    UA_NodeId_deleteMembers(&dataSetReader->identifier);
    UA_NodeId_deleteMembers(&dataSetReader->linkedReaderGroup);
    /* Remove DataSetReader from group */
    LIST_REMOVE(dataSetReader, listEntry);
    /* Free memory allocated for DataSetReader */
    UA_free(dataSetReader);
}

UA_StatusCode
UA_Server_processNetworkMessage(UA_Server *server, UA_NetworkMessage *pMsg,
                                UA_PubSubConnection *pConnection) {
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    if(!pMsg || !pConnection)
        return UA_STATUSCODE_BADINVALIDARGUMENT;

    /* To Do Handle multiple DataSetMessage for one NetworkMessage */
    /* To Do The condition pMsg->dataSetClassIdEnabled
     * Here some filtering is possible */

    UA_DataSetReader *dataSetReader;
    retval = getReaderFromIdentifier(server, pMsg, &dataSetReader, pConnection);
    if(retval != UA_STATUSCODE_GOOD) {
        return retval;
    }

    UA_LOG_DEBUG(&server->config.logger, UA_LOGCATEGORY_SERVER,
                 "DataSetReader found with PublisherId");

    UA_Byte anzDataSets = 1;
    if(pMsg->payloadHeaderEnabled)
        anzDataSets = pMsg->payloadHeader.dataSetPayloadHeader.count;
    for(UA_Byte iterator = 0; iterator < anzDataSets; iterator++) {
        UA_LOG_DEBUG(&server->config.logger, UA_LOGCATEGORY_SERVER, "Process Msg with DataSetReader!");
        UA_Server_DataSetReader_process(server, dataSetReader,
                                        &pMsg->payload.dataSetPayload.dataSetMessages[iterator]);
    }

    /* To Do Handle when dataSetReader parameters are null for publisherId
     * and zero for WriterGroupId and DataSetWriterId */
    return UA_STATUSCODE_GOOD;
}
void RPITempValueTest()
{
        float HumiArrTest[2];
        HumiArrTest[0] = FloatRPIHumi;
        HumiArrTest[1] = FloatRPIHumi2;
        
        //char sourcetimestamp[15];
        char dataname[15] = "Humidity";
        char pubip[15] = "172.21.35.10";

        if(HumiArrTest[0] != HumiArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Humidity %f ", HumiArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;
            ftime(&timebuffer);
            struct tm *now = localtime(&timebuffer.time);
            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);
            //시간함수소스

            //파일 입출력소스
            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPIHumi);
            fclose(fp);
            //파일 입출력소스
        }
}
void RPIHumiValueTest()
{
        float HumiArrTest[2];
        HumiArrTest[0] = FloatRPIHumi;
        HumiArrTest[1] = FloatRPIHumi2;
        
        //char sourcetimestamp[15];
        char dataname[15] = "Humidity";
        char pubip[15] = "172.21.35.10";

        if(HumiArrTest[0] != HumiArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Humidity %f ", HumiArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;
            ftime(&timebuffer);
            struct tm *now = localtime(&timebuffer.time);
            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);
            //시간함수소스

            //파일 입출력소스
            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPIHumi);
            fclose(fp);
            //파일 입출력소스
        }
}

void RPIGasValueTest()
{
        float GasArrTest[2];
        GasArrTest[0] = FloatRPIGas;
        GasArrTest[1] = FloatRPIGas2;

        //char sourcetimestamp[15];
        char dataname[15] = "Gas";
        char pubip[15] = "172.21.35.10";

        if(GasArrTest[0] != GasArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Gas %f ", GasArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //printf("%u:%u:%u.%03u / %u:%u:%u.%03u\n", st.hour, st.min, st.sec, st.milliSec, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm);
            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPIGas);
            fclose(fp);  
        }
}
void RPIIRValueTest()
{
        uint32_t IRArrTest[2];
        IRArrTest[0] = UIntRPIIR;
        IRArrTest[1] = UIntRPIIR2;

        //char sourcetimestamp[15];
        char dataname[15] = "Infrared";
        char pubip[15] = "172.21.35.10";

        if(IRArrTest[0] != IRArrTest[1])
        {
             UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 IR %d ", IRArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //printf("%u:%u:%u.%03u / %u:%u:%u.%03u\n", st.hour, st.min, st.sec, st.milliSec, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm);
            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, UIntRPIIR);
            fclose(fp);  
        }
}
void RPISoundValueTest()
{
        uint32_t SoundArrTest[2];
        SoundArrTest[0] = UIntRPISound;
        SoundArrTest[1] = UIntRPISound2;

        //char sourcetimestamp[15];
        char dataname[15] = "Sound";
        char pubip[15] = "172.21.35.10";

        if(SoundArrTest[0] != SoundArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Sound %d ", SoundArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, UIntRPISound);
            fclose(fp);  
        }   
}

void RPIAccelXValueTest()
{
        double AccelXArrTest[2];
        AccelXArrTest[0] = DoubleRPIAccelX;
        AccelXArrTest[1] = DoubleRPIAccelX2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationX";
        char pubip[15] = "172.21.35.10";

        if(AccelXArrTest[0] != AccelXArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Acceleration X %f", AccelXArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIAccelX);
            fclose(fp);
        }  
}
void RPIAccelYValueTest()
{
        double AccelYArrTest[2];
        AccelYArrTest[0] = DoubleRPIAccelY;
        AccelYArrTest[1] = DoubleRPIAccelY2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationY";
        char pubip[15] = "172.21.35.10";

        if(AccelYArrTest[0] != AccelYArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Acceleration Y %f", AccelYArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIAccelY);
            fclose(fp);
        }  
}
void RPIAccelZValueTest()
{
        double AccelZArrTest[2];
        AccelZArrTest[0] = DoubleRPIAccelZ;
        AccelZArrTest[1] = DoubleRPIAccelZ2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationZ";
        char pubip[15] = "172.21.35.10";

        if(AccelZArrTest[0] != AccelZArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Acceleration Z %f", AccelZArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIAccelY);
            fclose(fp);
        }  
}

void RPIGyroXValueTest()
{
        double GyroXArrTest[2];
        GyroXArrTest[0] = DoubleRPIGyroX;
        GyroXArrTest[1] = DoubleRPIGyroX2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeX";
        char pubip[15] = "172.21.35.10";

        if(GyroXArrTest[0] != GyroXArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Gyroscope X %f", GyroXArrTest[0]);

                        //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIGyroX);
            fclose(fp);
        }  
}
void RPIGyroYValueTest()
{
        double GyroYArrTest[2];
        GyroYArrTest[0] = DoubleRPIGyroY;
        GyroYArrTest[1] = DoubleRPIGyroY2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeY";
        char pubip[15] = "172.21.35.10";

        if(GyroYArrTest[0] != GyroYArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Gyroscope Y %f", GyroYArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIGyroY);
            fclose(fp);
        }  
}
void RPIGyroZValueTest()
{
        double GyroZArrTest[2];
        GyroZArrTest[0] = DoubleRPIGyroZ;
        GyroZArrTest[1] = DoubleRPIGyroZ2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeZ";
        char pubip[15] = "172.21.35.10";

        if(GyroZArrTest[0] != GyroZArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.10 Gyroscope Z %f", GyroZArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPIGyroZ);
            fclose(fp);
        }  
}

void BoolKitRedCheckTest()
{
    bool KitRedCheckTest[2];
    KitRedCheckTest[0] = BoolKitRedCheck;
    KitRedCheckTest[1] = BoolKitRedCheck2;

    //char sourcetimestamp[15];
    char dataname[15] = "KitRedCheck";
    char pubip[15] = "172.21.35.17";

    if(KitRedCheckTest[0] != KitRedCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Red Check %s", KitRedCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitRedCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void DoubleKitMotorTempTest()
{
    double KitMotorTempTest[2];
    KitMotorTempTest[0] = DoubleKitMotorTemp;
    KitMotorTempTest[1] = DoubleKitMotorTemp2;

    //char sourcetimestamp[15];
    char dataname[20] = "KitMotorTemperature";
    char pubip[15] = "172.21.35.17";

    if(KitMotorTempTest[0] != KitMotorTempTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Motor Temp %f", KitMotorTempTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleKitMotorTemp);
            fclose(fp);
    }
}
void BoolKitChromeCheckTest()
{
    bool KitChromeCheckTest[2];
    KitChromeCheckTest[0] = BoolKitChromeCheck;
    KitChromeCheckTest[1] = BoolKitChromeCheck2;

    //char sourcetimestamp[15];
    char dataname[15] = "KitChromeCheck";
    char pubip[15] = "172.21.35.17";

    if(KitChromeCheckTest[0] != KitChromeCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Chrome Check %s", KitChromeCheckTest[0] ?  "true" : "false");

        //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitChromeCheckTest[0] ?  "true" : "false");
            fclose(fp);

        
    }
}
void BoolKitBlackCheckTest()
{
    bool KitBlackCheckTest[2];
    KitBlackCheckTest[0] = BoolKitBlackCheck;
    KitBlackCheckTest[1] = BoolKitBlackCheck2;

    //char sourcetimestamp[15];
    char dataname[15] = "KitBlackCheck";
    char pubip[15] = "172.21.35.17";

    if(KitBlackCheckTest[0] != KitBlackCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Black Check %s", KitBlackCheckTest[0] ?  "true" : "false");

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitBlackCheckTest[0] ?  "true" : "false");
            fclose(fp);
    }
}
void BoolKitMentosCheckTest()
{
    bool KitMentosCheckTest[2];
    KitMentosCheckTest[0] = BoolKitMentosCheck;
    KitMentosCheckTest[1] = BoolKitMentosCheck2;

    //char sourcetimestamp[15];
    char dataname[15] = "KitMentosCheck";
    char pubip[15] = "172.21.35.17";

    if(KitMentosCheckTest[0] != KitMentosCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Mentos Check %s", KitMentosCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitMentosCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void BoolKitHousingLocationCheckTest()
{
    bool KitHousingLocationCheckTest[2];
    KitHousingLocationCheckTest[0] = BoolKitHousingLocationCheck;
    KitHousingLocationCheckTest[1] = BoolKitHousingLocationCheck2;

    //char sourcetimestamp[15];
    char dataname[24] = "KitHousingLocationCheck";
    char pubip[15] = "172.21.35.17";

    if(KitHousingLocationCheckTest[0] != KitHousingLocationCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Housing Location Check %s", KitHousingLocationCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitHousingLocationCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void BoolKitChromeSeperationCheckTest()
{
    bool KitChromeSeperationCheckTest[2];
    KitChromeSeperationCheckTest[0] = BoolKitChromeSeperationCheck;
    KitChromeSeperationCheckTest[1] = BoolKitChromeSeperationCheck2;

    //char sourcetimestamp[15];
    char dataname[25] = "KitChromeSeperationCheck";
    char pubip[15] = "172.21.35.17";

    if(KitChromeSeperationCheckTest[0] != KitChromeSeperationCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Chrome Seperation Check %s", KitChromeSeperationCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitChromeSeperationCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void BoolKitRedSeperationCheckTest()
{
    bool KitRedSeperationCheckTest[2];
    KitRedSeperationCheckTest[0] = BoolKitRedSeperationCheck;
    KitRedSeperationCheckTest[1] = BoolKitRedSeperationCheck2;

    //char sourcetimestamp[15];
    char dataname[23] = "KitRedSeperationCheck";
    char pubip[15] = "172.21.35.17";

    if(KitRedSeperationCheckTest[0] != KitRedSeperationCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Red Seperation Check %s", KitRedSeperationCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitRedSeperationCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void BoolKitBlackSeperationCheckTest()
{
    bool KitBlackSeperationCheckTest[2];
    KitBlackSeperationCheckTest[0] = BoolKitBlackSeperationCheck;
    KitBlackSeperationCheckTest[1] = BoolKitBlackSeperationCheck2;

    //char sourcetimestamp[15];
    char dataname[25] = "KitBlackSeperationCheck";
    char pubip[15] = "172.21.35.17";

    if(KitBlackSeperationCheckTest[0] != KitBlackSeperationCheckTest[1])
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.17 Black Seperation Check %s", KitBlackSeperationCheckTest[0] ?  "true" : "false");

        //시간함수소스
        struct timeb timebuffer;

        ftime(&timebuffer);

        struct tm *now = localtime(&timebuffer.time);

        //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

        //시간함수소스

        FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
        if(fp==NULL)
        {
            printf("FILE OPEN ERROR !!\n");
        }
        else
        {
            //printf("파일 생성 성공 \n");
        }
        fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %s\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, KitBlackSeperationCheckTest[0] ?  "true" : "false");
        fclose(fp);
    }
}
void RPI2GasValueTest()
{
        float RPI2GasArrTest[2];
        RPI2GasArrTest[0] = FloatRPI2Gas;
        RPI2GasArrTest[1] = FloatRPI2Gas2;

        //char sourcetimestamp[15];
        char dataname[15] = "Gas";
        char pubip[15] = "172.21.35.15";

        if(RPI2GasArrTest[0] != RPI2GasArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Gas %f ", RPI2GasArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPI2Gas);
            fclose(fp);
        }
}
void RPI2SoundValueTest()
{
        uint32_t SoundArrTest[2];
        SoundArrTest[0] = UIntRPI2Sound;
        SoundArrTest[1] = UIntRPI2Sound2;

        //char sourcetimestamp[15];
        char dataname[15] = "Sound";
        char pubip[15] = "172.21.35.15";

        if(SoundArrTest[0] != SoundArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Sound %d ", SoundArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, UIntRPI2Sound);
            fclose(fp);  
        }   
}

void RPI2IRValueTest()
{
        uint32_t IRArrTest[2];
        IRArrTest[0] = UIntRPI2IR;
        IRArrTest[1] = UIntRPI2IR2;

        //char sourcetimestamp[15];
        char dataname[15] = "IR";
        char pubip[15] = "172.21.35.15";

        if(IRArrTest[0] != IRArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 IR %d ", IRArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, UIntRPI2IR);
            fclose(fp);  
        }   
}

void RPI2TempValueTest()
{
        float TempArrTest[2];
        TempArrTest[0] = FloatRPI2Temp;
        TempArrTest[1] = FloatRPI2Temp2;

        //char sourcetimestamp[15];
        char dataname[15] = "Temperature";
        char pubip[15] = "172.21.35.15";

        if(TempArrTest[0] != TempArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Temperature %f ", TempArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //printf("%u:%u:%u.%03u / %u:%u:%u.%03u\n", st.hour, st.min, st.sec, st.milliSec, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm);
            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPI2Temp);
            fclose(fp);
        }
}

void RPI2HumiValueTest()
{
        float HumiArrTest[2];
        HumiArrTest[0] = FloatRPI2Humi;
        HumiArrTest[1] = FloatRPI2Humi2;
        
        //char sourcetimestamp[15];
        char dataname[15] = "Humidity";
        char pubip[15] = "172.21.35.15";

        if(HumiArrTest[0] != HumiArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Humidity %f ", HumiArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;
            ftime(&timebuffer);
            struct tm *now = localtime(&timebuffer.time);
            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);
            //시간함수소스

            //파일 입출력소스
            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, FloatRPI2Humi);
            fclose(fp);
            //파일 입출력소스
        }
}
void RPI2AccelXValueTest()
{
        double AccelXArrTest[2];
        AccelXArrTest[0] = DoubleRPI2AccelX;
        AccelXArrTest[1] = DoubleRPI2AccelX2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationX";
        char pubip[15] = "172.21.35.15";

        if(AccelXArrTest[0] != AccelXArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Acceleration X %f", AccelXArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2AccelX);
            fclose(fp);
        }  
}

void RPI2AccelYValueTest()
{
        double AccelYArrTest[2];
        AccelYArrTest[0] = DoubleRPI2AccelY;
        AccelYArrTest[1] = DoubleRPI2AccelY2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationY";
        char pubip[15] = "172.21.35.15";

        if(AccelYArrTest[0] != AccelYArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Acceleration Y %f", AccelYArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2AccelY);
            fclose(fp);
        }  
}

void RPI2AccelZValueTest()
{
        double AccelZArrTest[2];
        AccelZArrTest[0] = DoubleRPI2AccelZ;
        AccelZArrTest[1] = DoubleRPI2AccelZ2;

        //char sourcetimestamp[15];
        char dataname[15] = "AccelerationZ";
        char pubip[15] = "172.21.35.15";

        if(AccelZArrTest[0] != AccelZArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Acceleration Z %f", AccelZArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2AccelY);
            fclose(fp);
        }  
}

void RPI2GyroXValueTest()
{
        double GyroXArrTest[2];
        GyroXArrTest[0] = DoubleRPI2GyroX;
        GyroXArrTest[1] = DoubleRPI2GyroX2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeX";
        char pubip[15] = "172.21.35.15";

        if(GyroXArrTest[0] != GyroXArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Gyroscope X %f", GyroXArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2GyroX);
            fclose(fp);
        }  
}
void RPI2GyroYValueTest()
{
        double GyroYArrTest[2];
        GyroYArrTest[0] = DoubleRPIGyroY;
        GyroYArrTest[1] = DoubleRPIGyroY2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeY";
        char pubip[15] = "172.21.35.15";

        if(GyroYArrTest[0] != GyroYArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Gyroscope Y %f", GyroYArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2GyroY);
            fclose(fp);
        }  
}

void RPI2GyroZValueTest()
{
        double GyroZArrTest[2];
        GyroZArrTest[0] = DoubleRPI2GyroZ;
        GyroZArrTest[1] = DoubleRPI2GyroZ2;

        //char sourcetimestamp[15];
        char dataname[15] = "GyroscopeZ";
        char pubip[15] = "172.21.35.15";

        if(GyroZArrTest[0] != GyroZArrTest[1])
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Subscribed 172.21.35.15 Gyroscope Z %f", GyroZArrTest[0]);

            //시간함수소스
            struct timeb timebuffer;

            ftime(&timebuffer);

            struct tm *now = localtime(&timebuffer.time);

            //UA_DateTimeStruct st = UA_DateTime_toStruct(value->sourceTimestamp);

            //시간함수소스

            FILE *fp = fopen("/home/open62541/test62541/log/logtest.txt", "a");
            if(fp==NULL)
            {
                printf("FILE OPEN ERROR !!\n");
            }
            else
            {
                //printf("파일 생성 성공 \n");
            }
            fprintf(fp, "%u-%u-%u %u:%u:%u.%03u / %s / %s / %f\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, timebuffer.millitm, pubip, dataname, DoubleRPI2GyroZ);
            fclose(fp);
        }  
}

#endif /* UA_ENABLE_PUBSUB */
