<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="copynode">
    <xsl:variable name="node-to-copy" select="@source"/>
    <xsl:copy-of select="//*[name()=$node-to-copy]"/>
  </xsl:template>

</xsl:stylesheet>
