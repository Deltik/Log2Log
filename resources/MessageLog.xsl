<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- localized strings -->
<xsl:variable name='ColumnHeader_Date'>Date</xsl:variable>
<xsl:variable name='ColumnHeader_Time'>Time</xsl:variable>
<xsl:variable name='ColumnHeader_From'>From</xsl:variable>
<xsl:variable name='ColumnHeader_To'>To</xsl:variable>
<xsl:variable name='ColumnHeader_Message'>Message</xsl:variable>

<!-- variables -->
<xsl:variable name='Debug'>0</xsl:variable>

<xsl:variable name='TableStyle'>font-family:Segoe UI; font-size:75%; text-align:left; vertical-align:top; table-layout:fixed</xsl:variable>
<xsl:variable name='GutterStyle'>width:2ex</xsl:variable>
<xsl:variable name='HeaderStyle'>border-bottom:1 solid black</xsl:variable>

<xsl:variable name='UseZebraStripe'>1</xsl:variable>
<xsl:variable name='ZebraStripeStyle'>background-color:#e0edff</xsl:variable>

<xsl:variable name='MostRecentSessionFirst'>0</xsl:variable>


<xsl:template match="Log">

    <html dir='ltr'>
    <head>
        <title>
            Message Log for <xsl:value-of select="@LogonName"/>
            <xsl:if test="$Debug = 1"> (Debug)</xsl:if>
        </title>

        <xsl:if test="$Debug = 1">
            <span style="font-family:trebuchet ms; font-size:120%">
                Debug Version
            </span>
            <hr/>
        </xsl:if>
    </head>

    <body style='margin:0'>

        <table id='BodyTable' style="{$TableStyle}" cellspacing='0'>

            <xsl:if test="$Debug = 1">
                <col style="vertical-align:top; width:5ex;"/>
                <col style='{$GutterStyle}' />
            </xsl:if>

            <col style="width:16ex;"/>
            <col style='{$GutterStyle}' />
            <col style="width:16ex;"/>
            <col style='{$GutterStyle}' />
            <col style="width:21ex;"/>
            <col style='{$GutterStyle}' />
            <col style="width:21ex;"/>
            <col style='{$GutterStyle}' />
            <col style="width:70ex;"/>

            <thead>
                <tr>
                    <xsl:if test="$Debug = 1">
                        <th style="{$HeaderStyle}">SID</th>
                        <th/>
                    </xsl:if>
                    <th style="{$HeaderStyle}">
                        <xsl:value-of select="$ColumnHeader_Date"/>
                    </th>
                    <th/>
                    <th style="{$HeaderStyle}">
                        <xsl:value-of select="$ColumnHeader_Time"/>
                    </th>
                    <th/>
                    <th style="{$HeaderStyle}">
                        <xsl:value-of select="$ColumnHeader_From"/>
                    </th>
                    <th/>
                    <th style="{$HeaderStyle}">
                        <xsl:value-of select="$ColumnHeader_To"/>
                    </th>
                    <th/>
                    <th style="{$HeaderStyle}">
                        <xsl:value-of select="$ColumnHeader_Message"/>
                    </th>
                </tr>
            </thead>

            <tbody style='vertical-align:top'>
                <xsl:choose>

                    <!-- newest session first -->
                    <xsl:when test="$MostRecentSessionFirst = 1">
                        <xsl:apply-templates>
                            <xsl:sort select='@SessionID' order='descending' data-type='number'/>
                            <xsl:sort select='@DateTime'  order='ascending'/>
                        </xsl:apply-templates>
                    </xsl:when>

                    <!-- oldest session first -->
                    <xsl:otherwise>
                        <xsl:apply-templates>
                            <xsl:sort select='@SessionID' order='ascending' data-type='number'/>
                            <xsl:sort select='@DateTime'  order='ascending'/>
                        </xsl:apply-templates>
                    </xsl:otherwise>

                </xsl:choose>
            </tbody>
        </table>
    </body>
    </html>

</xsl:template>


<xsl:template match="Message">
    <tr>
        <xsl:call-template name="CommonMessageProcessing" />

        <td> <xsl:apply-templates select="From/User"/> </td>
        <td/>
        <td> <xsl:apply-templates select="To/User"/> </td>
        <td/>
        <td>
            <span>
                <xsl:value-of select="Text"/>
            </span>
        </td>
    </tr>
</xsl:template>


<xsl:template match="Invitation|InvitationResponse|Join|Leave">
    <tr>
        <xsl:call-template name="CommonMessageProcessing" />

        <td/>  <!-- From -->
        <td/>
        <td/>  <!-- To -->
        <td/>
        <td>
            <span>
                <xsl:value-of select="Text"/>
            </span>
        </td>
    </tr>
</xsl:template>


<xsl:template match="User">
    <!-- add a comma before all but the first user -->
    <xsl:if test="position() != 1">, </xsl:if>

    <xsl:value-of select="@FriendlyName"/>
</xsl:template>


<xsl:template name="CommonMessageProcessing">
    <!-- zebra-stripe the sessions -->
    <xsl:if test="$UseZebraStripe = 1">
        <xsl:if test="(@SessionID mod 2) = 1">
            <xsl:attribute name="style">
                <xsl:value-of select="$ZebraStripeStyle"/>
            </xsl:attribute>
        </xsl:if>
    </xsl:if>

    <xsl:if test="$Debug = 1">
        <td> <xsl:value-of select="@SessionID"/> </td>
        <td/>
    </xsl:if>

    <td> <xsl:value-of select="@Date"/> </td>
    <td/>
    <td> <xsl:value-of select="@Time"/> </td>
    <td/>
</xsl:template>


</xsl:stylesheet>
