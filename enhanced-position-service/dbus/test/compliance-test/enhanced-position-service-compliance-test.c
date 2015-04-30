/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* Component Name: EnhancedPositionService
* Author: Marco Residori <marco.residori@xse.de>
*
* Copyright (C) 2013, XS Embedded GmbH
* 
* License:
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Note:
* This test application was written using the code examples
* provided at http://www.matthew.ath.cx/misc/dbus as a reference.
*
* @licence end@
**************************************************************************/

#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define LAT 32  //value taken from GENIVI API
#define LON 33  //value taken from GENIVI API
#define ALT 34  //value taken from GENIVI API

const char * DBUS_INTERFACE = "org.genivi.positioning.EnhancedPosition";
const char * DBUS_PATH = "/org/genivi/positioning/EnhancedPosition";
const char * DBUS_SOURCE = "org.genivi.positioning.EnhancedPositionClient";
const char * DBUS_DESTINATION = "org.genivi.positioning.EnhancedPosition";
const char * DBUS_METHOD_GET_DATA = "GetData";

/**
 * This method emulates the API method GetData
 */
bool getPositioningData() 
{
    DBusMessage* msg;
    DBusMessageIter args_iter;
    DBusMessageIter data_iter;
    DBusMessageIter msg_iter;
    DBusMessageIter value_iter;
    DBusMessageIter array_iter;
    DBusConnection* conn;
    DBusError err;
    DBusPendingCall* pending;
    double latitude;
    double longitude;
    double altitude; //should be of type int (the PoC will have to be corrected)
    int ret;

    //initialise the errors
    dbus_error_init(&err);

    //connect to the session bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)) 
    { 
        fprintf(stderr, "connection error (%s)\n", err.message); 
        dbus_error_free(&err);
    }

    if(conn == NULL) 
    { 
        return false; 
    }

    //request name on the bus (optional)
    ret = dbus_bus_request_name(conn, DBUS_SOURCE, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if(dbus_error_is_set(&err)) 
    { 
        fprintf(stderr, "dbus name error (%s)\n", err.message); 
        dbus_error_free(&err);
    }

    if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) 
    { 
        return false; 
    }

    //create a new method call and check for errors
    msg = dbus_message_new_method_call(DBUS_DESTINATION, //destination
                                       DBUS_PATH, //object
                                       DBUS_INTERFACE, //interface
                                       DBUS_METHOD_GET_DATA); //method name
    if(NULL == msg) 
    { 
        fprintf(stderr, "message null\n");
        return false; 
    }

    dbus_uint16_t array[2];
    int size = 3;
    array[0]=LAT;
    array[1]=LON;
    array[2]=ALT;
    const dbus_uint16_t *v_ARRAY = array;

    //append arguments
    dbus_message_iter_init_append(msg, &args_iter);

    if(!dbus_message_iter_open_container(&args_iter, DBUS_TYPE_ARRAY,"q" , &data_iter))
    {
        fprintf(stderr, "out of memory!\n");
        return false; 
    }

    if(!dbus_message_iter_append_fixed_array(&data_iter, DBUS_TYPE_UINT16, &v_ARRAY, size))
    {
        fprintf(stderr, "out Of Memory!\n");
        return false; 
    }

    if(!dbus_message_iter_close_container(&args_iter, &data_iter))
    {
        fprintf(stderr, "out of memory!\n");
        return false; 
    }
 
    //send message and get a handle for a reply
    if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) // -1 is the default timeout
    { 
        fprintf(stderr, "out of memory!\n"); 
        return false; 
    }

    if(pending == NULL) 
    { 
        fprintf(stderr, "pending call is null\n"); 
        return false;  
    }
    dbus_connection_flush(conn);
   
    dbus_message_unref(msg);
   
    //block until we recieve a reply
    dbus_pending_call_block(pending);

    //get the reply message
    msg = dbus_pending_call_steal_reply(pending);
    if(msg == NULL) 
    {
        fprintf(stderr, "reply null\n"); 
        return false; 
    }

    //free the pending message handle
    dbus_pending_call_unref(pending);

    if(!dbus_message_iter_init(msg, &msg_iter)) 
    {
        fprintf(stderr, "format error\n");
        return false; 
    }

    //get the dictionary a{qv} 
    if (dbus_message_iter_get_arg_type (&msg_iter) != DBUS_TYPE_ARRAY ||
        dbus_message_iter_get_element_type (&msg_iter) !=
        DBUS_TYPE_DICT_ENTRY)
    {
        fprintf(stderr, "format error\n");
        return false; 
    }
 
    for (dbus_message_iter_recurse(&msg_iter, &array_iter);
         dbus_message_iter_get_arg_type (&array_iter) != DBUS_TYPE_INVALID;
         dbus_message_iter_next (&array_iter))
    {
        DBusMessageIter dict_iter;
        dbus_uint16_t key;
 
        //check that the array entry is a dict entry 
        if (dbus_message_iter_get_arg_type (&array_iter) != DBUS_TYPE_DICT_ENTRY)
        {
            return false; 
        }

        //recurse in dict entry  
        dbus_message_iter_recurse (&array_iter, &dict_iter);
 
        if(dbus_message_iter_get_arg_type(&dict_iter) != DBUS_TYPE_UINT16)
        {
  	        fprintf(stderr, "format error: wrong data type %c\n", (char)dbus_message_iter_get_arg_type(&dict_iter));
            return false;
        }
 
        dbus_message_iter_get_basic(&dict_iter, &key);
        dbus_message_iter_next(&dict_iter);
       
        switch(key)
        {
            case LAT:
            //read variant value 
            if (dbus_message_iter_get_arg_type (&dict_iter) == DBUS_TYPE_VARIANT)
            {
                dbus_message_iter_recurse(&dict_iter, &value_iter);

                if(dbus_message_iter_get_arg_type(&value_iter) != DBUS_TYPE_DOUBLE)
                {
                    fprintf(stderr, "format error: wrong data type %c\n", (char)dbus_message_iter_get_arg_type(&value_iter));
                    return false;
                }

                dbus_message_iter_get_basic(&value_iter, (void *)&latitude);
                printf("latitude=%f\n",latitude);

                if(latitude > 90 || latitude < -90)
                {
                   fprintf(stderr, "latitude value out of range\n");
                   return false;
                }
            }
            break;

            case LON:
            //read variant value 
            if (dbus_message_iter_get_arg_type (&dict_iter) == DBUS_TYPE_VARIANT)
            {
                dbus_message_iter_recurse(&dict_iter, &value_iter);

                if(dbus_message_iter_get_arg_type(&value_iter) != DBUS_TYPE_DOUBLE)
                {
                    fprintf(stderr, "format error: wrong data type %c\n", (char)dbus_message_iter_get_arg_type(&value_iter));
                    return false;
                }

                dbus_message_iter_get_basic(&value_iter, (void *)&longitude);
                printf("longitude=%f\n",longitude);

                if(longitude > 180 || longitude < -180)
                {
                    fprintf(stderr, "longitude value out of range\n");
                    return false;
                }
            }
            break;

            case ALT:
            //read following variant value 
            if(dbus_message_iter_get_arg_type (&dict_iter) == DBUS_TYPE_VARIANT)
            {
                dbus_message_iter_recurse(&dict_iter, &value_iter);

                if (dbus_message_iter_get_arg_type(&value_iter) != DBUS_TYPE_DOUBLE) //should be DBUS_TYPE_INT32
                {
                    fprintf(stderr, "format error: wrong data type %c\n", (char)dbus_message_iter_get_arg_type(&value_iter));
                    return false;
                }

                dbus_message_iter_get_basic(&value_iter, (void *)&altitude);
                printf("altitude=%f\n",altitude); //should be 'd' instead (the PoC will have to be corrected)

                if(altitude > 9000 || longitude < -1000)
                {
                    fprintf(stderr, "altitude value out of range\n");
                    return false;
                }
            }
            break;

            default:
                //do nothing
            break;
        }
    }
 
    //free reply and close connection
    dbus_message_unref(msg);   

    //dbus_connection_close(conn);

    return true;
}

int main(int argc, char** argv)
{
    printf("Starting gnss-service-compliance-test...\n");
  
    if(getPositioningData() == false)
    {
       printf("TEST_FAILED\n");
       return EXIT_FAILURE;
    }

    printf("TEST_PASSED\n");
    return EXIT_SUCCESS;
}



