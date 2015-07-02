/////////////////////////////////////////////////////////////////////////////////
//
// @licence app begin@
// SPDX-License-Identifier: MPL-2.0
//
// Component Name: PositionWebService
//
// Author: Marco Residori
//
// Copyright (C) 2014, XS Embedded GmbH
// 
// License:
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
// this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// @licence end@
//
/////////////////////////////////////////////////////////////////////////////////

var Position = {
       latitude: 0, 
       longitude: 0,
       altitude: 0,
       accuracy: 0,
       altitudeAccuracy: 0,
       heading: 0,
       speed: 0
    };

var PositionError = {
       code: 0,
       message: ""
}

//flags
var isGetPending = false;
var isWatchActive = false;

//callbacks
var PositionCallback; 
var PositionErrorCallback; 
var WatchCallback; 
var WatchErrorCallback; 

window.onload = function() {
    console.log( "Get plugin");
    plugin = document.getElementById('plugin');
}

function getCurrentPosition(successCallback, errorCallback, options){
   console.log( "getCurrentPosition()" );

   isGetPending = true;

   PositionCallback = successCallback;
   PositionErrorCallback = errorCallback;
}

function watchPosition(successCallback, errorCallback, options)
{
    plugin.addEventListener('PositionUpdate', PositionUpdate, false);
    isWatchActive = true;

    WatchCallback = successCallback;
    WatchErrorCallback = errorCallback;
}

function clearWatch()
{
    plugin.removeEventListener('PositionUpdate', PositionUpdate, false);
    isWatchActive = false;
    WatchCallback = 'undefined';
    WatchErrorCallback = 'undefined';
}

function PositionUpdate(data) {
   console.log( "PositionUpdate" );

   var pos = plugin.GetPosition();

   if (data.indexOf("LATITUDE") > -1) {
       Position.latitude = pos["LATITUDE"].toFixed(4);
   }

   if (data.indexOf("LONGITUDE") > -1) {
       Position.longitude = pos["LONGITUDE"].toFixed(4);
   }

   if (data.indexOf("ALTITUDE") > -1) {
       Position.altitude = pos["ALTITUDE"].toFixed(2);
   }

   if (data.indexOf("SPEED") > -1) {
       Position.speed = pos["SPEED"].toFixed(4);
   }

   if (isGetPending == true)
   {
      console.log( "calling PositionCallback");
      isGetPending = false;
      PositionCallback(Position);   
   }
   
   if (isWatchActive == true)
   {
      console.log( "calling WatchCallback");
      WatchCallback(Position);   
   }
}




