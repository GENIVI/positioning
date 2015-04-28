<?xml version="1.0" encoding="UTF-8"?>
<!-- ************************************************************************
* Component Name: Navit POC
* Author: Martin Schaller <martin.schaller@it-schaller.de>
*
* Copyright (C) 2012, GENIVI Alliance, Inc.
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as 
* published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
************************************************************************ -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version="1.0">
	<xsl:output method="text" encoding="iso-8859-15"/>
	<xsl:template match="constants">
		<xsl:variable name="constants" select="translate(@name,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
/* This is an automatically generated file, do not edit */

<!-- enum EPOS_<xsl:value-of select="$constants"/>_Constants {<xsl:for-each select="id">
	POS_<xsl:value-of select="$constants"/>_<xsl:value-of select="translate(@name,'-','_')"/> = <xsl:value-of select="@value"/>,</xsl:for-each>
};
 -->

enum GENIVI_<xsl:value-of select="$constants"/>_Constants {<xsl:for-each select="id">
	GENIVI_<xsl:value-of select="$constants"/>_<xsl:value-of select="translate(@name,'-','_')"/> = <xsl:value-of select="@value"/>,</xsl:for-each>
};
  </xsl:template>
</xsl:stylesheet>
