<?xml version="1.0" encoding="ISO-8859-15"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

<!-- SPDX-License-Identifier: MPL-2.0
     Component Name: EnhancedPositionService
     Compliance Level: Abstract Component
     Copyright (C) 2012, BMW Car IT GmbH, Continental Automotive GmbH, PCA Peugeot Citroën, XS Embedded GmbH
     License:
     This Source Code Form is subject to the terms of the
     Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
     this file, You can obtain one at http://mozilla.org/MPL/2.0/.
-->

<xsl:output method="xml" version="1.0" encoding="iso-8859-15" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" indent="yes"/>

<xsl:template match="/">
  <html>
  	
  	<!-- head -->
    <head>
      <title>DBUS Introspection Data</title>
      <style type="text/css">
        body { color: black; background-color: white } 
        h1 { font-family: sans-serif }
        ul { list-style-type: none; margin-bottom: 10px }
        li { font-family: sans-serif }
        .keyword { font-style: italic }
        .title { font-style: italic; color: blue }
		    .line { font-style: italic; font-size:8pt; color: green}
		    .version { font-family: monospace }
      </style>
    </head>
    
    <!-- body -->
    <body>
    	<!-- definitions -->
    	<xsl:apply-templates select="constants"/> 
    </body>
  </html>
</xsl:template>

<xsl:template match="constants"> 
  <div class="constants">  	
    <h1>
    	<span class="keyword">constants</span><xsl:text> </xsl:text>
      <span class="title"><xsl:value-of select="@name"/></span>
      <xsl:text> </xsl:text>
      <span class="version">version</span>
      <xsl:text> </xsl:text>
      <span class="version"><xsl:value-of select="version"/></span>
    </h1>  
    
    <xsl:apply-templates select="doc"/>
    
    <xsl:for-each select="id">
		  <hr/>
        <li>
         <xsl:apply-templates select="doc"/>
         <xsl:value-of select="@name"/> = <xsl:value-of select="@value"/>
        </li>
    </xsl:for-each>
  </div>
</xsl:template>

<xsl:template match="doc"> 
  <li>
    <span class="line">
      <xsl:for-each select="line">
        <xsl:value-of select="."/><xsl:element name="br"/>
	    </xsl:for-each>
	  </span>
  </li>
</xsl:template>

</xsl:stylesheet>
