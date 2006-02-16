<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="2.0"
    xmlns:lxslt="http://xml.apache.org/xslt">

  <!--
       Use DashboardStamp as a parameter, default to most recent
       The proper flags to Xalan are in the form -PARAM DashboardStamp "string('foo')"
       -->
  <xsl:param name="DashboardStamp" select="string('MostRecentResults-Nightly')"/>
  <xsl:variable name="DashboardDir" select="concat('../../../../Dashboard/', $DashboardStamp)"/>
  <xsl:param name="TestDocDir">.</xsl:param>
  <xsl:preserve-space elements="xsl:text"/>
  <xsl:include href="DashboardConfig.xsl"/>
  <xsl:output method="html"/>
  <xsl:template match="/">
    Build.html is deliberately empty. Separate BuildError.html and BuildWarning.html files are generated.
    <xsl:call-template name="Summary"/>
    <xsl:call-template name="ErrorLog"/>
    <xsl:call-template name="WarningLog"/>
  </xsl:template>

  <xsl:template name="ErrorLog">
    <xsl:variable name="ErrorLogURI" select="concat('file:///', $TestDocDir, '/BuildError.html' )"/>
    <xsl:result-document href="{$ErrorLogURI}" >
      <xsl:call-template name="DashboardHeader">
        <xsl:with-param name="Title">Build Errors <xsl:value-of select="Site/@Name"/> -- <xsl:value-of select="Site/@BuildName"/></xsl:with-param>
        <xsl:with-param name="IconDir">../../../../Icons</xsl:with-param>
        <xsl:with-param name="DashboardDir" select="$DashboardDir"/>
      </xsl:call-template>
     <p><b><xsl:text>Site:</xsl:text></b><xsl:value-of select="Site/@Name"/></p><p>
<b><xsl:text>Build Name:</xsl:text></b><xsl:value-of select="Site/@BuildName"/></p> 
Found <xsl:value-of select="count(Site/Build/Error)"/> Errors<br/>
<p><a href="BuildWarning.html">Warnings</a> are here.</p>
      <xsl:for-each select="Site/Build/Error">
        <xsl:call-template name="FormatContext"/>
      </xsl:for-each>
      <xsl:call-template name="DashboardFooter">
	<xsl:with-param name="IconDir">../../../../Icons</xsl:with-param>
      </xsl:call-template>
    </xsl:result-document>
  </xsl:template>

  <xsl:template name="WarningLog">
    <xsl:variable name="WarningLogURI" select="concat('file:///', $TestDocDir, '/BuildWarning.html' )"/>
    <xsl:result-document href="{$WarningLogURI}" >
      <xsl:call-template name="DashboardHeader">
        <xsl:with-param name="Title">Build Warnings <xsl:value-of select="Site/@Name"/> -- <xsl:value-of select="Site/@BuildName"/></xsl:with-param>
        <xsl:with-param name="IconDir">../../../../Icons</xsl:with-param>
        <xsl:with-param name="DashboardDir" select="$DashboardDir"/>
      </xsl:call-template>
    <p><b><xsl:text>Site:</xsl:text></b><xsl:value-of select="Site/@Name"/></p><p>
<b><xsl:text>Build Name:</xsl:text></b><xsl:value-of select="Site/@BuildName"/></p>       
 Found <xsl:value-of select="count(Site/Build/Warning)"/> Warnings<br/>
<p><a href="BuildError.html">Errors</a> are here.</p>
      <xsl:for-each select="Site/Build/Warning">
        <xsl:call-template name="FormatContext"/>
      </xsl:for-each>
      <xsl:call-template name="DashboardFooter">
	<xsl:with-param name="IconDir">../../../../Icons</xsl:with-param>
      </xsl:call-template>
    </xsl:result-document>
  </xsl:template>

  <xsl:template name="FormatContext">
  <hr/>
  <h3>
    <A>
      <xsl:attribute name="Name"><xsl:value-of select="BuildLogLine"/></xsl:attribute>
      Build Log line <xsl:value-of select="BuildLogLine"/>
    </A>
  </h3>

  <br/>
  <xsl:choose>
    <xsl:when test="SourceFile != ''">
      File: 
      <b>
        <xsl:value-of select="SourceFile"/>
      </b>
      Line: 
      <b>
        <xsl:value-of select="SourceLineNumber"/>
      </b> 
      <xsl:if test="$CVSWebURL!=''">
      <a><xsl:attribute name="href"><xsl:value-of select="$CVSWebURL"/><xsl:value-of select="SourceFile"/><xsl:choose><xsl:when test="$CVSROOT!=''">?cvsroot=<xsl:value-of select="$CVSROOT"/>&amp;annotate=HEAD</xsl:when><xsl:otherwise>?annotate=HEAD</xsl:otherwise></xsl:choose>#<xsl:value-of select="SourceLineNumber"/></xsl:attribute>CVS</a>
      </xsl:if>
      <xsl:if test="Url !=''">
      <p><a><xsl:attribute name="href"><xsl:value-of select="Url"/></xsl:attribute>
      <xsl:choose><xsl:when test="UrlName !=''"><xsl:value-of select="UrlName"/></xsl:when><xsl:otherwise>
      <xsl:value-of select="Url"/></xsl:otherwise></xsl:choose>
      </a></p>
      </xsl:if>
      <xsl:if test="$UseDoxygen='ON'">
      <xsl:if test="SourceFileTail != ''">
	<xsl:text>     and            </xsl:text>
        <a>
          <xsl:attribute name="href"><xsl:value-of select="$DoxygenURL"/><xsl:value-of select="translate ( SourceFileTail, '.', '_' )"/>-source.html</xsl:attribute>
        Doxygen
        </a><xsl:text> access </xsl:text>
      </xsl:if>
     </xsl:if>
    </xsl:when>
  </xsl:choose>
  <pre><xsl:value-of select="PreContext"/><b><xsl:value-of select="Text"/></b>
<xsl:value-of select="PostContext"/></pre>
  <xsl:if test="RepeatCount != '0'">
    This error was repeated another <xsl:value-of select="RepeatCount"/> times.
  </xsl:if>
</xsl:template>


<xsl:template name="Summary">
  <xsl:variable name="uri" select="concat('file:///', $TestDocDir, '/BuildSummary.xml' )" />
  <xsl:result-document href="{$uri}" >
    <Build>
      <SiteName><xsl:value-of select="Site/@Name"/></SiteName>
      <BuildName><xsl:value-of select="Site/@BuildName"/></BuildName>
      <BuildStamp><xsl:value-of select="Site/@BuildStamp"/></BuildStamp>
      <StartDateTime><xsl:value-of select="Site/Build/StartDateTime"/></StartDateTime>

      <WarningCount><xsl:value-of select="count(Site/Build/Warning)"/></WarningCount>
      <ErrorCount><xsl:value-of select="count(Site/Build/Error)"/></ErrorCount>
      <EndDateTime><xsl:value-of select="Site/Build/EndDateTime"/></EndDateTime>
      <ElapsedMinutes><xsl:value-of select="Site/Build/ElapsedMinutes"/></ElapsedMinutes>
      <Generator><xsl:value-of select="Site/@Generator"/></Generator>
      <Files>
        <xsl:for-each select="Site/Build/Error">
          <xsl:if test="SourceFile">
            <File Type='Error'>
              <Name><xsl:value-of select="SourceFile"/></Name>
              <SourceLineNumber><xsl:value-of select="SourceLineNumber"/></SourceLineNumber>
              <Text><xsl:value-of select="Text"/></Text>
              <BuildLogLine><xsl:value-of select="BuildLogLine"/></BuildLogLine>
            </File>
          </xsl:if>
        </xsl:for-each>
        <xsl:for-each select="Site/Build/Warning">
          <xsl:if test="SourceFile">
            <File Type='Warning'>
              <Name><xsl:value-of select="SourceFile"/></Name>
              <SourceLineNumber><xsl:value-of select="SourceLineNumber"/></SourceLineNumber>
              <Text><xsl:value-of select="Text"/></Text>
              <BuildLogLine><xsl:value-of select="BuildLogLine"/></BuildLogLine>
            </File>
          </xsl:if>
        </xsl:for-each>
      </Files>
    </Build>
  </xsl:result-document>
</xsl:template>
</xsl:stylesheet>
