/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information.
 *
 * Copyright (c) 2019 Kalycito Infotech Private Limited
 */

/**
 * IMPORTANT ANNOUNCEMENT
 * The PubSub Subscriber API is currently not finished. This example can be used
 * to receive and display values that are published by tutorial_pubsub_publish
 * example in the TargetVariables of Subscriber Information Model .
 */

 /*
 RPI001 : 172.21.35.10
 RPI002 : 172.21.35.15
 33.215 IPC랑 소스 똑같이 해서 붙여넣기 하면 됨
 */

#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pubsub_udp.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/types_generated.h>

#include <open62541/plugin/pubsub_udp.h>
//#include "ua_pubsub.h"

#ifdef UA_ENABLE_PUBSUB_ETH_UADP
#include <open62541/plugin/pubsub_ethernet.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

UA_NodeId connectionIdentifier;
UA_NodeId readerGroupIdentifier;
UA_NodeId readerIdentifier;
UA_DataSetReaderConfig readerConfig;

UA_NodeId connectionIdentifier2;
UA_NodeId readerGroupIdentifier2;
UA_NodeId readerIdentifier2;
UA_DataSetReaderConfig readerConfig2;

UA_NodeId connectionIdentifier3;
UA_NodeId readerGroupIdentifier3;
UA_NodeId readerIdentifier3;
UA_DataSetReaderConfig readerConfig3;

static void RPI001fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData);
static void RPI002fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData);
static void KIT210fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData);

/* Add new connection to the server */
static UA_StatusCode
RPI001addPubSubConnection(UA_Server *server, UA_String *transportProfile,
                    UA_NetworkAddressUrlDataType *networkAddressUrl) {
    if((server == NULL) || (transportProfile == NULL) ||
        (networkAddressUrl == NULL)) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    /* Configuration creation for the connection */
    UA_PubSubConnectionConfig connectionConfig;

    /* Connection 1 */
    memset (&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING("UDPMC Connection 1");
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
    connectionConfig.publisherId.numeric = UA_UInt32_random (); //이거는 아무거나 해줘도 상관없다
    UA_Server_addPubSubConnection (server, &connectionConfig, &connectionIdentifier);
    UA_PubSubConnection_regist(server, &connectionIdentifier);
}

/* Add ReaderGroup to the created connection */
static UA_StatusCode
RPI001addReaderGroup(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_ReaderGroupConfig readerGroupConfig;
    memset (&readerGroupConfig, 0, sizeof(UA_ReaderGroupConfig));
    readerGroupConfig.name = UA_STRING("ReaderGroup1");
    retval |= UA_Server_addReaderGroup(server, connectionIdentifier, &readerGroupConfig,
                                       &readerGroupIdentifier);
    return retval;
}

/* Add DataSetReader to the ReaderGroup */
static UA_StatusCode
RPI001addDataSetReader(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    memset (&readerConfig, 0, sizeof(UA_DataSetReaderConfig));
    readerConfig.name = UA_STRING("DataSet Reader 1");
    /* Parameters to filter which DataSetMessage has to be processed
     * by the DataSetReader */
    /* The following parameters are used to show that the data published by
     * tutorial_pubsub_publish.c is being subscribed and is being updated in
     * the information model */
    UA_UInt16 publisherIdentifier = 2000;
    readerConfig.publisherId.type = &UA_TYPES[UA_TYPES_UINT16];
    readerConfig.publisherId.data = &publisherIdentifier;
    readerConfig.writerGroupId    = 100;
    readerConfig.dataSetWriterId  = 60000;

    /* Setting up Meta data configuration in DataSetReader */
    RPI001fillTestDataSetMetaData(&readerConfig.dataSetMetaData);
    retval |= UA_Server_addDataSetReader(server, readerGroupIdentifier, &readerConfig,
                                         &readerIdentifier);
    return retval;
}

/* Set SubscribedDataSet type to TargetVariables data type
 * Add subscribedvariables to the DataSetReader */
static UA_StatusCode
RPI001addSubscribedDataSet (UA_Server *server, UA_NodeId dataSetReaderId) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_NodeId folderId;
    UA_String folderName = readerConfig.dataSetMetaData.name;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    UA_QualifiedName folderBrowseName;
    if(folderName.length > 0) {
        oAttr.displayName.locale = UA_STRING ("en-US");
        oAttr.displayName.text = folderName;
        folderBrowseName.namespaceIndex = 1;
        folderBrowseName.name = folderName;
    }
    else {
        oAttr.displayName = UA_LOCALIZEDTEXT ("en-US", "Subscribed Variables");
        folderBrowseName = UA_QUALIFIEDNAME (1, "Subscribed Variables");
    }

    UA_Server_addObjectNode (server, UA_NODEID_NULL,
                             UA_NODEID_NUMERIC (0, UA_NS0ID_OBJECTSFOLDER),
                             UA_NODEID_NUMERIC (0, UA_NS0ID_ORGANIZES),
                             folderBrowseName, UA_NODEID_NUMERIC (0,
                             UA_NS0ID_BASEOBJECTTYPE), oAttr, NULL, &folderId);

    retval |= UA_Server_DataSetReader_addTargetVariables (server, &folderId,
                                                          dataSetReaderId,
                                                          0);
    UA_free(readerConfig.dataSetMetaData.fields);
    return retval;
}

/* Add ReaderGroup to the created connection */
/* Add new connection to the server */
static UA_StatusCode
KIT210addPubSubConnection2(UA_Server *server, UA_String *transportProfile,
                    UA_NetworkAddressUrlDataType *networkAddressUrl) {
    if((server == NULL) || (transportProfile == NULL) ||
        (networkAddressUrl == NULL)) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    /* Configuration creation for the connection */
    UA_PubSubConnectionConfig connectionConfig;

    /* Connection 2 */
    memset (&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING("UDPMC Connection 2");
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
    connectionConfig.publisherId.numeric = UA_UInt32_random (); //이거는 아무거나 해줘도 상관없다
    UA_Server_addPubSubConnection (server, &connectionConfig, &connectionIdentifier2);
    UA_PubSubConnection_regist(server, &connectionIdentifier2);
}

static UA_StatusCode
KIT210addReaderGroup(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    UA_ReaderGroupConfig readerGroupConfig;

    memset (&readerGroupConfig, 0, sizeof(UA_ReaderGroupConfig));
    readerGroupConfig.name = UA_STRING("ReaderGroup2");
    UA_Server_addReaderGroup(server, connectionIdentifier2, &readerGroupConfig,
                                       &readerGroupIdentifier2);

    return retval;

}

/* Add DataSetReader to the ReaderGroup */
static UA_StatusCode
KIT210addDataSetReader(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    /* DataSetReader 2 */
    memset (&readerConfig, 0, sizeof(UA_DataSetReaderConfig));
    readerConfig.name = UA_STRING("DataSet Reader 2");
    UA_UInt16 publisherIdentifier = 2001;
    readerConfig.publisherId.type = &UA_TYPES[UA_TYPES_UINT16];
    readerConfig.publisherId.data = &publisherIdentifier;
    readerConfig.writerGroupId    = 101;
    readerConfig.dataSetWriterId  = 60001;

    KIT210fillTestDataSetMetaData(&readerConfig.dataSetMetaData); //이거 없으면 서버에 변수 안생김

    UA_Server_addDataSetReader(server, readerGroupIdentifier2, &readerConfig, //이거 없애면 SUB 시작이 안됨
                                         &readerIdentifier2);
    return retval;
}

/* Set SubscribedDataSet type to TargetVariables data type
 * Add subscribedvariables to the DataSetReader */
static UA_StatusCode
KIT210addSubscribedDataSet (UA_Server *server, UA_NodeId dataSetReaderId) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_NodeId folderId;
    UA_String folderName = readerConfig.dataSetMetaData.name;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    UA_QualifiedName folderBrowseName;
    if(folderName.length > 0) {
        oAttr.displayName.locale = UA_STRING ("en-US");
        oAttr.displayName.text = folderName;
        folderBrowseName.namespaceIndex = 1;
        folderBrowseName.name = folderName;
    }
    else {
        oAttr.displayName = UA_LOCALIZEDTEXT ("en-US", "Subscribed Variables 2");
        folderBrowseName = UA_QUALIFIEDNAME (1, "Subscribed Variables 2");
    }

    UA_Server_addObjectNode (server, UA_NODEID_NULL,
                             UA_NODEID_NUMERIC (0, UA_NS0ID_OBJECTSFOLDER),
                             UA_NODEID_NUMERIC (0, UA_NS0ID_ORGANIZES),
                             folderBrowseName, UA_NODEID_NUMERIC (0,
                             UA_NS0ID_BASEOBJECTTYPE), oAttr, NULL, &folderId);

    retval |= UA_Server_DataSetReader_addTargetVariables (server, &folderId,
                                                          dataSetReaderId,
                                                          0);                                                          
    UA_free(readerConfig.dataSetMetaData.fields);
    return retval;
}

//RPI001 세팅
/* Add ReaderGroup to the created connection */
/* Add new connection to the server */
static UA_StatusCode
RPI002addPubSubConnection(UA_Server *server, UA_String *transportProfile,
                    UA_NetworkAddressUrlDataType *networkAddressUrl) {
    if((server == NULL) || (transportProfile == NULL) ||
        (networkAddressUrl == NULL)) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    /* Configuration creation for the connection */
    UA_PubSubConnectionConfig connectionConfig;

    /* Connection 3 */
    memset (&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING("UDPMC Connection 3");
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
    connectionConfig.publisherId.numeric = UA_UInt32_random (); //이거는 아무거나 해줘도 상관없다
    UA_Server_addPubSubConnection (server, &connectionConfig, &connectionIdentifier3);
    UA_PubSubConnection_regist(server, &connectionIdentifier3);
}

static UA_StatusCode
RPI002addReaderGroup(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    UA_ReaderGroupConfig readerGroupConfig;

    memset (&readerGroupConfig, 0, sizeof(UA_ReaderGroupConfig));
    readerGroupConfig.name = UA_STRING("ReaderGroup3");
    UA_Server_addReaderGroup(server, connectionIdentifier3, &readerGroupConfig,
                                       &readerGroupIdentifier3);

    return retval;

}

/* Add DataSetReader to the ReaderGroup */
static UA_StatusCode
RPI002addDataSetReader(UA_Server *server) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    /* DataSetReader 3 */
    memset (&readerConfig, 0, sizeof(UA_DataSetReaderConfig));
    readerConfig.name = UA_STRING("DataSet Reader 3");
    UA_UInt16 publisherIdentifier = 2002;
    readerConfig.publisherId.type = &UA_TYPES[UA_TYPES_UINT16];
    readerConfig.publisherId.data = &publisherIdentifier;
    readerConfig.writerGroupId    = 102;
    readerConfig.dataSetWriterId  = 60002;

    RPI002fillTestDataSetMetaData(&readerConfig.dataSetMetaData); //이거 없으면 서버에 변수 안생김

    UA_Server_addDataSetReader(server, readerGroupIdentifier3, &readerConfig, //이거 없애면 SUB 시작이 안됨
                                         &readerIdentifier3);
    return retval;
}

/* Set SubscribedDataSet type to TargetVariables data type
 * Add subscribedvariables to the DataSetReader */
static UA_StatusCode
RPI002addSubscribedDataSet (UA_Server *server, UA_NodeId dataSetReaderId) {
    if(server == NULL) {
        return UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_NodeId folderId;
    UA_String folderName = readerConfig.dataSetMetaData.name;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    UA_QualifiedName folderBrowseName;
    if(folderName.length > 0) {
        oAttr.displayName.locale = UA_STRING ("en-US");
        oAttr.displayName.text = folderName;
        folderBrowseName.namespaceIndex = 1;
        folderBrowseName.name = folderName;
    }
    else {
        oAttr.displayName = UA_LOCALIZEDTEXT ("en-US", "Subscribed Variables 3");
        folderBrowseName = UA_QUALIFIEDNAME (1, "Subscribed Variables 3");
    }

    UA_Server_addObjectNode (server, UA_NODEID_NULL,
                             UA_NODEID_NUMERIC (0, UA_NS0ID_OBJECTSFOLDER),
                             UA_NODEID_NUMERIC (0, UA_NS0ID_ORGANIZES),
                             folderBrowseName, UA_NODEID_NUMERIC (0,
                             UA_NS0ID_BASEOBJECTTYPE), oAttr, NULL, &folderId);

    retval |= UA_Server_DataSetReader_addTargetVariables (server, &folderId,
                                                          dataSetReaderId,
                                                          0);
    UA_free(readerConfig.dataSetMetaData.fields);
    return retval;
}

//RPI002 세팅
/* Define MetaData for TargetVariables */
static void RPI001fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData) {
    if(pMetaData == NULL) {
        return;
    }

     UA_DataSetMetaDataType_init (pMetaData);
    pMetaData->name = UA_STRING ("RPI DataSet");

    /* Static definition of number of fields size to 4 to create four different
     * targetVariables of distinct datatype
     * Currently the publisher sends only DateTime data type */
    pMetaData->fieldsSize = 11;
    pMetaData->fields = (UA_FieldMetaData*)UA_Array_new (pMetaData->fieldsSize,
                         &UA_TYPES[UA_TYPES_FIELDMETADATA]);

    /* GyroZ */
    UA_FieldMetaData_init (&pMetaData->fields[10]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[10].dataType);
    pMetaData->fields[10].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[10].name =  UA_STRING ("Gyroscope Z");
    pMetaData->fields[10].valueRank = -1; /* scalar */
    
    /* GyroY */
    UA_FieldMetaData_init (&pMetaData->fields[9]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[9].dataType);
    pMetaData->fields[9].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[9].name =  UA_STRING ("Gyroscope Y");
    pMetaData->fields[9].valueRank = -1; /* scalar */

    /* GyroX */
    UA_FieldMetaData_init (&pMetaData->fields[8]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[8].dataType);
    pMetaData->fields[8].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[8].name =  UA_STRING ("Gyroscope X");
    pMetaData->fields[8].valueRank = -1; /* scalar */

    /* AccelZ */
    UA_FieldMetaData_init (&pMetaData->fields[7]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[7].dataType);
    pMetaData->fields[7].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[7].name =  UA_STRING ("Acceleration Z");
    pMetaData->fields[7].valueRank = -1; /* scalar */

    /* AccelY */
    UA_FieldMetaData_init (&pMetaData->fields[6]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[6].dataType);
    pMetaData->fields[6].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[6].name =  UA_STRING ("Acceleration Y");
    pMetaData->fields[6].valueRank = -1; /* scalar */

    /* AccelX */
    UA_FieldMetaData_init (&pMetaData->fields[5]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[5].dataType);
    pMetaData->fields[5].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[5].name =  UA_STRING ("Acceleration X");
    pMetaData->fields[5].valueRank = -1; /* scalar */

    /* SOUND */
    UA_FieldMetaData_init (&pMetaData->fields[4]);
    UA_NodeId_copy(&UA_TYPES[UA_TYPES_UINT32].typeId,
                   &pMetaData->fields[4].dataType);
    pMetaData->fields[4].builtInType = UA_NS0ID_UINT32;
    pMetaData->fields[4].name =  UA_STRING ("Sound");
    pMetaData->fields[4].valueRank = -1; /* scalar */

    /* IR */
    UA_FieldMetaData_init (&pMetaData->fields[3]);
    UA_NodeId_copy(&UA_TYPES[UA_TYPES_UINT32].typeId,
                   &pMetaData->fields[3].dataType);
    pMetaData->fields[3].builtInType = UA_NS0ID_UINT32;
    pMetaData->fields[3].name =  UA_STRING ("Infrared");
    pMetaData->fields[3].valueRank = -1; /* scalar */

    /* Gas */
    UA_FieldMetaData_init (&pMetaData->fields[2]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[2].dataType);
    pMetaData->fields[2].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[2].name =  UA_STRING ("Gas");
    pMetaData->fields[2].valueRank = -1; /* scalar */

    /* TEMPERATURE */
    UA_FieldMetaData_init (&pMetaData->fields[1]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[1].dataType);
    pMetaData->fields[1].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[1].name =  UA_STRING ("Temperature");
    pMetaData->fields[1].valueRank = -1; /* scalar */

   /* HUMIDITY */
    UA_FieldMetaData_init (&pMetaData->fields[0]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[0].dataType);
    pMetaData->fields[0].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[0].name =  UA_STRING ("Humidity");
    pMetaData->fields[0].valueRank = -1; /* scalar */
    

    /*테스트용소스*/
    // // /* Int32 DataType */
    // UA_FieldMetaData_init (&pMetaData->fields[1]);
    // UA_NodeId_copy(&UA_TYPES[UA_TYPES_INT32].typeId,
    //                &pMetaData->fields[1].dataType);
    // pMetaData->fields[1].builtInType = UA_NS0ID_INT32;
    // pMetaData->fields[1].name =  UA_STRING ("Int32");
    // pMetaData->fields[1].valueRank = -1; /* scalar */

    // /* DateTime DataType */
    // UA_FieldMetaData_init (&pMetaData->fields[1]);
    // UA_NodeId_copy (&UA_TYPES[UA_TYPES_DATETIME].typeId,
    //                 &pMetaData->fields[1].dataType);
    // pMetaData->fields[1].builtInType = UA_NS0ID_DATETIME;
    // pMetaData->fields[1].name =  UA_STRING ("DateTime");
    // pMetaData->fields[1].valueRank = -1; /* scalar */

    // /* Boolean DataType */
    // UA_FieldMetaData_init (&pMetaData->fields[3]);
    // UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
    //                 &pMetaData->fields[3].dataType);
    // pMetaData->fields[3].builtInType = UA_NS0ID_BOOLEAN;
    // pMetaData->fields[3].name =  UA_STRING ("BoolToggle");
    // pMetaData->fields[3].valueRank = -1; /* scalar */
}

/* Define MetaData for TargetVariables */
static void KIT210fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData) {
    if(pMetaData == NULL) {
        return;
    }

    UA_DataSetMetaDataType_init (pMetaData);
    pMetaData->name = UA_STRING ("TrainigKit DataSet");

    /* Static definition of number of fields size to 4 to create four different
     * targetVariables of distinct datatype
     * Currently the publisher sends only DateTime data type */
    pMetaData->fieldsSize = 9;
    pMetaData->fields = (UA_FieldMetaData*)UA_Array_new (pMetaData->fieldsSize,
                         &UA_TYPES[UA_TYPES_FIELDMETADATA]);

    /* KIT Black Seperation Check */
    UA_FieldMetaData_init (&pMetaData->fields[8]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[8].dataType);
    pMetaData->fields[8].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[8].name =  UA_STRING ("Kit BlackSeperationCHK");
    pMetaData->fields[8].valueRank = -1; /* scalar */

    /* KIT Red Seperation Check */
    UA_FieldMetaData_init (&pMetaData->fields[7]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[7].dataType);
    pMetaData->fields[7].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[7].name =  UA_STRING ("Kit RedSeperationCHK");
    pMetaData->fields[7].valueRank = -1; /* scalar */


    /* KIT Chrome Seperation Check */
    UA_FieldMetaData_init (&pMetaData->fields[6]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[6].dataType);
    pMetaData->fields[6].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[6].name =  UA_STRING ("Kit ChromeSeperationCHK");
    pMetaData->fields[6].valueRank = -1; /* scalar */

    /* KIT Housing Location Check */
    UA_FieldMetaData_init (&pMetaData->fields[5]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[5].dataType);
    pMetaData->fields[5].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[5].name =  UA_STRING ("Kit HousingLocationCHK");
    pMetaData->fields[5].valueRank = -1; /* scalar */

    /* KIT Mentos Check */
    UA_FieldMetaData_init (&pMetaData->fields[4]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[4].dataType);
    pMetaData->fields[4].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[4].name =  UA_STRING ("Kit MentosCHK");
    pMetaData->fields[4].valueRank = -1; /* scalar */

    /* KIT Black Housing Check */
    UA_FieldMetaData_init (&pMetaData->fields[3]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[3].dataType);
    pMetaData->fields[3].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[3].name =  UA_STRING ("Kit BlackHousingCHK");
    pMetaData->fields[3].valueRank = -1; /* scalar */

    /* KIT Chrome Housing Check */
    UA_FieldMetaData_init (&pMetaData->fields[2]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[2].dataType);
    pMetaData->fields[2].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[2].name =  UA_STRING ("Kit ChromeHousingCHK");
    pMetaData->fields[2].valueRank = -1; /* scalar */

    /* KIT Temperature */
    UA_FieldMetaData_init (&pMetaData->fields[1]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[1].dataType);
    pMetaData->fields[1].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[1].name =  UA_STRING ("Motor Temperature");
    pMetaData->fields[1].valueRank = -1; /* scalar */

    /* KIT Red Housing Check */
    UA_FieldMetaData_init (&pMetaData->fields[0]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
                    &pMetaData->fields[0].dataType);
    pMetaData->fields[0].builtInType = UA_NS0ID_BOOLEAN;
    pMetaData->fields[0].name =  UA_STRING ("Kit RedHousingCHK");
    pMetaData->fields[0].valueRank = -1; /* scalar */

    // /* KIT RED Housing Check */
    // UA_FieldMetaData_init (&pMetaData->fields[2]);
    // UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
    //                 &pMetaData->fields[2].dataType);
    // pMetaData->fields[2].builtInType = UA_NS0ID_BOOLEAN;
    // pMetaData->fields[2].name =  UA_STRING ("Kit RedHousingCHK");
    // pMetaData->fields[2].valueRank = -1; /* scalar */

    // /* KIT Chrome Housing Check */
    // UA_FieldMetaData_init (&pMetaData->fields[1]);
    // UA_NodeId_copy (&UA_TYPES[UA_TYPES_BOOLEAN].typeId,
    //                 &pMetaData->fields[1].dataType);
    // pMetaData->fields[1].builtInType = UA_NS0ID_BOOLEAN;
    // pMetaData->fields[1].name =  UA_STRING ("Kit ChromeHousingCHK");
    // pMetaData->fields[1].valueRank = -1; /* scalar */

    // /* KIT Temperature */
    // UA_FieldMetaData_init (&pMetaData->fields[0]);
    // UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
    //                 &pMetaData->fields[0].dataType);
    // pMetaData->fields[0].builtInType = UA_NS0ID_DOUBLE;
    // pMetaData->fields[0].name =  UA_STRING ("Motor Temperature");
    // pMetaData->fields[0].valueRank = -1; /* scalar */

}
/* Define MetaData for TargetVariables */
static void RPI002fillTestDataSetMetaData(UA_DataSetMetaDataType *pMetaData) {
    if(pMetaData == NULL) {
        return;
    }

    UA_DataSetMetaDataType_init (pMetaData);
    pMetaData->name = UA_STRING ("RPI2 DataSet");

    /* Static definition of number of fields size to 4 to create four different
     * targetVariables of distinct datatype
     * Currently the publisher sends only DateTime data type */
    pMetaData->fieldsSize = 11;
    pMetaData->fields = (UA_FieldMetaData*)UA_Array_new (pMetaData->fieldsSize,
                         &UA_TYPES[UA_TYPES_FIELDMETADATA]);
  

    /* GyroZ */
    UA_FieldMetaData_init (&pMetaData->fields[10]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[10].dataType);
    pMetaData->fields[10].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[10].name =  UA_STRING ("Gyroscope Z");
    pMetaData->fields[10].valueRank = -1; /* scalar */

    
    /* GyroY */
    UA_FieldMetaData_init (&pMetaData->fields[9]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[9].dataType);
    pMetaData->fields[9].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[9].name =  UA_STRING ("Gyroscope Y");
    pMetaData->fields[9].valueRank = -1; /* scalar */


    /* GyroX */
    UA_FieldMetaData_init (&pMetaData->fields[8]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[8].dataType);
    pMetaData->fields[8].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[8].name =  UA_STRING ("Gyroscope X");
    pMetaData->fields[8].valueRank = -1; /* scalar */


    /* AccelZ */
    UA_FieldMetaData_init (&pMetaData->fields[7]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[7].dataType);
    pMetaData->fields[7].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[7].name =  UA_STRING ("Acceleration Z");
    pMetaData->fields[7].valueRank = -1; /* scalar */

    /* AccelY */
    UA_FieldMetaData_init (&pMetaData->fields[6]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[6].dataType);
    pMetaData->fields[6].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[6].name =  UA_STRING ("Acceleration Y");
    pMetaData->fields[6].valueRank = -1; /* scalar */

    /* AccelX */
    UA_FieldMetaData_init (&pMetaData->fields[5]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_DOUBLE].typeId,
                    &pMetaData->fields[5].dataType);
    pMetaData->fields[5].builtInType = UA_NS0ID_DOUBLE;
    pMetaData->fields[5].name =  UA_STRING ("Acceleration X");
    pMetaData->fields[5].valueRank = -1; /* scalar */

    /* HUMIDITY */
    UA_FieldMetaData_init (&pMetaData->fields[4]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[4].dataType);
    pMetaData->fields[4].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[4].name =  UA_STRING ("Humidity");
    pMetaData->fields[4].valueRank = -1; /* scalar */


    /* TEMPERATURE */
    UA_FieldMetaData_init (&pMetaData->fields[3]);
    UA_NodeId_copy(&UA_TYPES[UA_TYPES_FLOAT].typeId,
                   &pMetaData->fields[3].dataType);
    pMetaData->fields[3].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[3].name =  UA_STRING ("Temperature");
    pMetaData->fields[3].valueRank = -1; /* scalar */

    /* INFRAED */
    UA_FieldMetaData_init (&pMetaData->fields[2]);
    UA_NodeId_copy(&UA_TYPES[UA_TYPES_UINT32].typeId,
                   &pMetaData->fields[2].dataType);
    pMetaData->fields[2].builtInType = UA_NS0ID_UINT32;
    pMetaData->fields[2].name =  UA_STRING ("Infrared");
    pMetaData->fields[2].valueRank = -1; /* scalar */

    /* SOUND */
    UA_FieldMetaData_init (&pMetaData->fields[1]);
    UA_NodeId_copy(&UA_TYPES[UA_TYPES_UINT32].typeId,
                   &pMetaData->fields[1].dataType);
    pMetaData->fields[1].builtInType = UA_NS0ID_UINT32;
    pMetaData->fields[1].name =  UA_STRING ("Sound");
    pMetaData->fields[1].valueRank = -1; /* scalar */

    /* GAS */
    UA_FieldMetaData_init (&pMetaData->fields[0]);
    UA_NodeId_copy (&UA_TYPES[UA_TYPES_FLOAT].typeId,
                    &pMetaData->fields[0].dataType);
    pMetaData->fields[0].builtInType = UA_NS0ID_FLOAT;
    pMetaData->fields[0].name =  UA_STRING ("Gas");
    pMetaData->fields[0].valueRank = -1;
}

UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

static int
run(UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    /* Return value initialized to Status Good */
    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    UA_ServerConfig_setMinimal(config, 4840, NULL);

    /* Add the PubSub network layer implementation to the server config.
     * The TransportLayer is acting as factory to create new connections
     * on runtime. Details about the PubSubTransportLayer can be found inside the
     * tutorial_pubsub_connection */
    config->pubsubTransportLayers = (UA_PubSubTransportLayer *)
        UA_calloc(2, sizeof(UA_PubSubTransportLayer));
    if(!config->pubsubTransportLayers) {
        UA_Server_delete(server);
        return EXIT_FAILURE;
    }

    config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
    config->pubsubTransportLayersSize++;
#ifdef UA_ENABLE_PUBSUB_ETH_UADP
    config->pubsubTransportLayers[1] = UA_PubSubTransportLayerEthernet();
    config->pubsubTransportLayersSize++;
#endif

    /* API calls */
    /* Add PubSubConnection */
    retval |= RPI001addPubSubConnection(server, transportProfile, networkAddressUrl);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    /* Add ReaderGroup to the created PubSubConnection */
    retval |= RPI001addReaderGroup(server);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    /* Add DataSetReader to the created ReaderGroup */
    retval |= RPI001addDataSetReader(server);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    /* Add SubscribedVariables to the created DataSetReader */
    retval |= RPI001addSubscribedDataSet(server, readerIdentifier);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    /* API CALL 2 */
    retval |= KIT210addPubSubConnection2(server, transportProfile, networkAddressUrl);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    retval |= KIT210addReaderGroup(server);
    if (retval != UA_STATUSCODE_GOOD)
    {
        return EXIT_FAILURE;
    }
    retval |= KIT210addDataSetReader(server);
    if (retval != UA_STATUSCODE_GOOD)
    return EXIT_FAILURE;

    retval |= KIT210addSubscribedDataSet(server, readerIdentifier2);
    if (retval != UA_STATUSCODE_GOOD)
    return EXIT_FAILURE;

    /* API CALL 3 */
    retval |= RPI002addPubSubConnection(server, transportProfile, networkAddressUrl);
    if (retval != UA_STATUSCODE_GOOD)
        return EXIT_FAILURE;

    retval |= RPI002addReaderGroup(server);
    if (retval != UA_STATUSCODE_GOOD)
    {
        return EXIT_FAILURE;
    }
    retval |= RPI002addDataSetReader(server);
    if (retval != UA_STATUSCODE_GOOD)
    return EXIT_FAILURE;

    retval |= RPI002addSubscribedDataSet(server, readerIdentifier3);
    if (retval != UA_STATUSCODE_GOOD)
    return EXIT_FAILURE;

    retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void
usage(char *progname) {
    printf("usage: %s <uri> [device]\n", progname);
}

int main(int argc, char **argv) {
    UA_String transportProfile = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
    UA_NetworkAddressUrlDataType networkAddressUrl = {UA_STRING_NULL , UA_STRING("opc.udp://224.0.0.22:4840/")};
    if(argc > 1) {
        if(strcmp(argv[1], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if(strncmp(argv[1], "opc.udp://", 10) == 0) {
            networkAddressUrl.url = UA_STRING(argv[1]);
        } else if(strncmp(argv[1], "opc.eth://", 10) == 0) {
            transportProfile =
                UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-eth-uadp");
            if(argc < 3) {
                printf("Error: UADP/ETH needs an interface name\n");
                return EXIT_FAILURE;
            }

            networkAddressUrl.networkInterface = UA_STRING(argv[2]);
            networkAddressUrl.url = UA_STRING(argv[1]);
        } else {
            printf ("Error: unknown URI\n");
            return EXIT_FAILURE;
        }
    }

    return run(&transportProfile, &networkAddressUrl);
}
