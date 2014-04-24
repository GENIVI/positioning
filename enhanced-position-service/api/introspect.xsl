<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version="1.0">
  <!-- 
 Copyright (C) 2005 Lennart Poettering.

 Licensed under the Academic Free License version 2.1

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-->

  <!-- v0.1 26-06-2011 Marco Residori (XS Embedded GmbH): Added support for inline documentation (tags doc,line and version) -->
  <!-- v0.2 12-01-2012 Marco Residori (XS Embedded GmbH): Added support for inline documentation (tags status,type) -->
  <!-- v0.3 18-01-2012 Marco Residori (XS Embedded GmbH): Added support for inline documentation (tag error) -->
  <!-- v0.4 14-03-2012 Marco Residori (XS Embedded GmbH): Added support for property-type and property-access. Replaced tag 'type' with 'obligation' -->
  
  <!-- $Id$ -->
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
        .type { font-weight: bold; font-size: 10pt }
        .symbol { font-family: monospace }
        .title { font-style: italic; color: blue }
        .interface { padding: 10px; margin: 10px }
        
	.line { font-style: italic; font-size:8pt; color: green}
	.version { font-family: monospace }
	.status { font-weight: bold; font-size:6pt }
	.obligation { font-weight: bold; font-size:6pt }
	.error { font-style: italic; font-size:8pt; color: black}
      </style>
      </head>
      <!-- body -->
      <body>
        <!-- interfaces -->
        <xsl:for-each select="node/interface">
          <div class="interface">
            <h1>
              <span class="keyword">interface</span>
              <xsl:text> </xsl:text>
              <span class="title">
                <xsl:value-of select="@name"/>
              </span>
              <xsl:text> </xsl:text>
              <span class="version">version</span>
              <xsl:text> </xsl:text>
              <span class="version">
                <xsl:value-of select="version"/>
              </span>
            </h1>
            <ul>
              <xsl:apply-templates select="doc"/>
            </ul>
            <ul>
              <xsl:apply-templates select="annotation"/>
              <!-- methods, signals and properties -->
              <xsl:for-each select="method|signal|property">
                <hr/>
                <li>
                  <xsl:if test="status !=''">
                    <span class="status">[<xsl:value-of select="status"/>]  </span>
                  </xsl:if>
                  <xsl:if test="obligation !=''">
                    <span class="obligation">[<xsl:value-of select="obligation"/>]  </span>
                  </xsl:if>
                  <xsl:apply-templates select="doc"/>
                  <span class="keyword">
                    <xsl:value-of select="name()"/>
                  </span>
                  <xsl:text> </xsl:text>
                  <span class="symbol">
                    <xsl:value-of select="@name"/>
                  </span>
                  <xsl:text> </xsl:text>
                  <span class="keyword">
                    <xsl:value-of select="@access"/>
                  </span>
                  <xsl:text> </xsl:text>
                  <span class="type">
                    <xsl:value-of select="@type"/>
                  </span>
                  <xsl:text> </xsl:text>
                  <ul>
                    <xsl:apply-templates select="annotation"/>
                    <!-- arguments -->
                    <xsl:for-each select="arg">
                      <br/>
                      <xsl:apply-templates select="doc"/>
                      <li>
                        <span class="keyword">
                          <xsl:choose>
                            <xsl:when test="@direction !=''">
                              <xsl:value-of select="@direction"/>
                            </xsl:when>
                            <xsl:when test="name(..) ='signal'">
                            out
                          </xsl:when>
                            <xsl:otherwise>
                            in
                          </xsl:otherwise>
                          </xsl:choose>
                        </span>
                        <xsl:text> </xsl:text>
                        <span class="type">
                          <xsl:value-of select="@type"/>
                        </span>
                        <xsl:text> </xsl:text>
                        <span class="symbol">
                          <xsl:value-of select="@name"/>
                        </span>
                        <xsl:text> </xsl:text>
                      </li>
                    </xsl:for-each>
                    <br/>
                    <!-- errors -->
                    <xsl:apply-templates select="error"/>
                    <br/>
                  </ul>
                </li>
              </xsl:for-each>
            </ul>
          </div>
        </xsl:for-each>
      </body>
    </html>
  </xsl:template>
  <xsl:template match="annotation">
    <li>
      <span class="keyword">annotation</span>
      <code>
        <xsl:value-of select="@name"/>
      </code>
      <xsl:text> = </xsl:text>
      <code>
        <xsl:value-of select="@value"/>
      </code>
    </li>
  </xsl:template>
  <xsl:template match="doc">
    <li>
      <span class="line">
        <xsl:for-each select="line">
          <xsl:value-of select="."/>
          <xsl:element name="br"/>
        </xsl:for-each>
      </span>
    </li>
  </xsl:template>
  <xsl:template match="error">
    <li>
      <xsl:apply-templates select="doc"/>
      <span class="keyword">error</span>
      <xsl:text> </xsl:text>
      <span class="error">
        <code>
          <xsl:value-of select="@name"/>
        </code>
      </span>
      <xsl:element name="br"/>
      <br/>
    </li>
  </xsl:template>
</xsl:stylesheet>
