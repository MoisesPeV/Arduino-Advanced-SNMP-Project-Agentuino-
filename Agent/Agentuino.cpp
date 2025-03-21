/*
  Agentuino.cpp - An Arduino library for a lightweight SNMP Agent.
  Copyright (C) 2010 Eric C. Gionet <lavco_eg@hotmail.com>
  All rights reserved.

  @Edit: M. Pivovarsky <miroslav.pivovarsky@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

//
// sketch_aug23a
//

#include "Agentuino.h"
#include "EthernetUdp.h"

EthernetUDP Udp;

SNMP_API_STAT_CODES AgentuinoClass::begin() {
    // set community names
    _getCommName = "public";
    _setCommName = "public";
    //
    // set community name set/get sizes
    _setSize = strlen(_setCommName);
    _getSize = strlen(_getCommName);
    //
    // init UDP socket
    Udp.begin(SNMP_DEFAULT_PORT);
    //
    return SNMP_API_STAT_SUCCESS;
}

SNMP_API_STAT_CODES AgentuinoClass::begin(char *getCommName, char *setCommName, uint16_t port) {
    // set community name set/get sizes
    _setSize = strlen(setCommName);
    _getSize = strlen(getCommName);
    //
    // validate get/set community name sizes
    if (_setSize > SNMP_MAX_NAME_LEN + 1 || _getSize > SNMP_MAX_NAME_LEN + 1) {
        return SNMP_API_STAT_NAME_TOO_BIG;
    }
    //
    // set community names
    _getCommName = getCommName;
    _setCommName = setCommName;
    //
    // validate session port number
    if (port == NULL || port == 0) port = SNMP_DEFAULT_PORT;
    //
    // init UDP socket
    Udp.begin(port);

    return SNMP_API_STAT_SUCCESS;
}

void AgentuinoClass::listen(void) {
    // if bytes are available in receive buffer
    // and pointer to a function (delegate function)
    // isn't null, trigger the function
    Udp.parsePacket();
    if (Udp.available() && _callback != NULL) (*_callback)();
}

SNMP_API_STAT_CODES AgentuinoClass::requestPdu(SNMP_PDU *pdu) {
    char *community;
    // sequence length
    byte seqLen;
    // version
    byte verLen, verEnd;
    // community string
    byte comLen, comEnd;
    // pdu
    byte pduTyp, pduLen;
    byte ridLen, ridEnd;
    byte errLen, errEnd;
    byte eriLen, eriEnd;
    byte vblTyp, vblLen;
    byte vbiTyp, vbiLen;
    byte obiLen, obiEnd;
    byte valTyp, valLen, valEnd;
    byte i;
    //
    // set packet packet size (skip UDP header)
    _packetSize = Udp.available();
    //
    // reset packet array
    memset(_packet, 0, SNMP_MAX_PACKET_LEN);
    //
    // validate packet
    if (_packetSize != 0 && _packetSize > SNMP_MAX_PACKET_LEN) {
        //
        //SNMP_FREE(_packet);

        return SNMP_API_STAT_PACKET_TOO_BIG;
    }
    //
    // get UDP packet
    //Udp.parsePacket();
    Udp.read(_packet, _packetSize);
    // 	Udp.readPacket(_packet, _packetSize, _dstIp, &_dstPort);
    //
    // packet check 1
    if (_packet[0] != 0x30) {
        //
        //SNMP_FREE(_packet);

        return SNMP_API_STAT_PACKET_INVALID;
    }
    //
    // sequence length
    seqLen = _packet[1];
    // version
    verLen = _packet[3];
    verEnd = 3 + verLen;
    // community string
    comLen = _packet[verEnd + 2];
    comEnd = verEnd + 2 + comLen;
    // pdu
    pduTyp = _packet[comEnd + 1];
    pduLen = _packet[comEnd + 2];
    ridLen = _packet[comEnd + 4];
    ridEnd = comEnd + 4 + ridLen;
    errLen = _packet[ridEnd + 2];
    errEnd = ridEnd + 2 + errLen;
    eriLen = _packet[errEnd + 2];
    eriEnd = errEnd + 2 + eriLen;
    vblTyp = _packet[eriEnd + 1];
    vblLen = _packet[eriEnd + 2];
    vbiTyp = _packet[eriEnd + 3];
    vbiLen = _packet[eriEnd + 4];
    obiLen = _packet[eriEnd + 6];
    obiEnd = eriEnd + obiLen + 6;
    valTyp = _packet[obiEnd + 1];
    valLen = _packet[obiEnd + 2];
    valEnd = obiEnd + 2 + valLen;
    //
    // extract version
    pdu->version = 0;
    for (i = 0; i < verLen; i++) {
        pdu->version = (pdu->version << 8) | _packet[5 + i];
    }
    //
    // validate version
    //
    // pdu-type
    pdu->type = (SNMP_PDU_TYPES) pduTyp;
    _dstType = pdu->type;
    //
    // validate community size
    if (comLen > SNMP_MAX_NAME_LEN) {
        // set pdu error
        pdu->error = SNMP_ERR_TOO_BIG;
        //
        return SNMP_API_STAT_NAME_TOO_BIG;
    }
    //
    //
    // validate community name
    if (pdu->type == SNMP_PDU_SET && comLen == _setSize) {
        //
        for (i = 0; i < _setSize; i++) {
            if (_packet[verEnd + 3 + i] != (byte) _setCommName[i]) {
                // set pdu error
                pdu->error = SNMP_ERR_NO_SUCH_NAME;
                //
                return SNMP_API_STAT_NO_SUCH_NAME;
            }
        }
    } else if (pdu->type == SNMP_PDU_GET && comLen == _getSize) {
        //
        for (i = 0; i < _getSize; i++) {
            if (_packet[verEnd + 3 + i] != (byte) _getCommName[i]) {
                // set pdu error
                pdu->error = SNMP_ERR_NO_SUCH_NAME;
                //
                return SNMP_API_STAT_NO_SUCH_NAME;
            }
        }
    }else if ( pdu->type == SNMP_PDU_GET_NEXT && comLen == _getSize ) {
   //
    for ( i = 0; i < _getSize; i++ ) {
      if( _packet[verEnd + 3 + i] != (byte)_getCommName[i] ) {
        // set pdu error
        pdu->error = SNMP_ERR_NO_SUCH_NAME;
        //
        return SNMP_API_STAT_NO_SUCH_NAME;
      }
    }
    }else {
        // set pdu error
        pdu->error = SNMP_ERR_NO_SUCH_NAME;
        //
        return SNMP_API_STAT_NO_SUCH_NAME;
    }
    //
    //
    // extract reqiest-id 0x00 0x00 0x00 0x01 (4-byte int aka int32)
    pdu->requestId = 0;
    for (i = 0; i < ridLen; i++) {
        pdu->requestId = (pdu->requestId << 8) | _packet[comEnd + 5 + i];
    }
    //
    // extract error
    pdu->error = SNMP_ERR_NO_ERROR;
    int32_t err = 0;
    for (i = 0; i < errLen; i++) {
        err = (err << 8) | _packet[ridEnd + 3 + i];
    }
    pdu->error = (SNMP_ERR_CODES) err;
    //
    // extract error-index
    pdu->errorIndex = 0;
    for (i = 0; i < eriLen; i++) {
        pdu->errorIndex = (pdu->errorIndex << 8) | _packet[errEnd + 3 + i];
    }
    //
    //
    // validate object-identifier size
    if (obiLen > SNMP_MAX_OID_LEN) {
        // set pdu error
        pdu->error = SNMP_ERR_TOO_BIG;

        return SNMP_API_STAT_OID_TOO_BIG;
    }
    //
    // extract and contruct object-identifier
    memset(pdu->OID.data, 0, SNMP_MAX_OID_LEN);
    pdu->OID.size = obiLen;
    for (i = 0; i < obiLen; i++) {
        pdu->OID.data[i] = _packet[eriEnd + 7 + i];
    }
    //
    // value-type
    pdu->VALUE.syntax = (SNMP_SYNTAXES) valTyp;
    //
    // validate value size
    if (obiLen > SNMP_MAX_VALUE_LEN) {
        // set pdu error
        pdu->error = SNMP_ERR_TOO_BIG;

        return SNMP_API_STAT_VALUE_TOO_BIG;
    }
    //
    // value-size
    pdu->VALUE.size = valLen;
    //
    // extract value
    memset(pdu->VALUE.data, 0, SNMP_MAX_VALUE_LEN);
    for (i = 0; i < valLen; i++) {
        pdu->VALUE.data[i] = _packet[obiEnd + 3 + i];
    }
    //
    return SNMP_API_STAT_SUCCESS;
}

SNMP_API_STAT_CODES AgentuinoClass::responsePdu(SNMP_PDU *pdu) {
    int32_u u;
    byte i;
    //
    // Length of entire SNMP packet
    _packetPos = 0; // 23
    _packetSize = 25 + sizeof (pdu->requestId) + sizeof (pdu->error) + sizeof (pdu->errorIndex) + pdu->OID.size + pdu->VALUE.size;
    //
    memset(_packet, 0, SNMP_MAX_PACKET_LEN);
    //
    if (_dstType == SNMP_PDU_SET) {
        _packetSize += _setSize;
    } else {
        _packetSize += _getSize;
    }
    //
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_SEQUENCE; // type
    _packet[_packetPos++] = (byte) _packetSize - 2; // length
    //
    // SNMP version
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_INT; // type
    _packet[_packetPos++] = 0x01; // length
    _packet[_packetPos++] = 0x00; // value
    //
    // SNMP community string
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_OCTETS; // type
    if (_dstType == SNMP_PDU_SET) {
        _packet[_packetPos++] = (byte) _setSize; // length
        for (i = 0; i < _setSize; i++) {
            _packet[_packetPos++] = (byte) _setCommName[i];
        }
    } else {
        _packet[_packetPos++] = (byte) _getSize; // length
        for (i = 0; i < _getSize; i++) {
            _packet[_packetPos++] = (byte) _getCommName[i];
        }
    }
    //
    // SNMP PDU
    _packet[_packetPos++] = (byte) pdu->type;
    _packet[_packetPos++] = (byte) (sizeof (pdu->requestId) + sizeof ((int32_t) pdu->error) + sizeof (pdu->errorIndex) + pdu->OID.size + pdu->VALUE.size + 14);
    //
    // Request ID (size always 4 e.g. 4-byte int)
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_INT; // type
    _packet[_packetPos++] = (byte)sizeof (pdu->requestId);
    u.int32 = pdu->requestId;
    _packet[_packetPos++] = u.data[3];
    _packet[_packetPos++] = u.data[2];
    _packet[_packetPos++] = u.data[1];
    _packet[_packetPos++] = u.data[0];
    //
    // Error (size always 4 e.g. 4-byte int)
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_INT; // type
    _packet[_packetPos++] = (byte)sizeof ((int32_t) pdu->error);
    u.int32 = pdu->error;
    _packet[_packetPos++] = u.data[3];
    _packet[_packetPos++] = u.data[2];
    _packet[_packetPos++] = u.data[1];
    _packet[_packetPos++] = u.data[0];
    //
    // Error Index (size always 4 e.g. 4-byte int)
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_INT; // type
    _packet[_packetPos++] = (byte)sizeof (pdu->errorIndex);
    u.int32 = pdu->errorIndex;
    _packet[_packetPos++] = u.data[3];
    _packet[_packetPos++] = u.data[2];
    _packet[_packetPos++] = u.data[1];
    _packet[_packetPos++] = u.data[0];
    //
    // Varbind List
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_SEQUENCE; // type
    _packet[_packetPos++] = (byte) (pdu->OID.size + pdu->VALUE.size + 6); //4
    //
    // Varbind
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_SEQUENCE; // type
    _packet[_packetPos++] = (byte) (pdu->OID.size + pdu->VALUE.size + 4); //2
    //
    // ObjectIdentifier
    _packet[_packetPos++] = (byte) SNMP_SYNTAX_OID; // type
    _packet[_packetPos++] = (byte) (pdu->OID.size);
    for (i = 0; i < pdu->OID.size; i++) {
        _packet[_packetPos++] = pdu->OID.data[i];
    }
    //
    // Value
    _packet[_packetPos++] = (byte) pdu->VALUE.syntax; // type
    _packet[_packetPos++] = (byte) (pdu->VALUE.size);
    for (i = 0; i < pdu->VALUE.size; i++) {
        _packet[_packetPos++] = pdu->VALUE.data[i];
    }
    //
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_packet, _packetSize);
    Udp.endPacket();
    //	Udp.write(_packet, _packetSize, _dstIp, _dstPort);
    //
    return SNMP_API_STAT_SUCCESS;
}

void AgentuinoClass::onPduReceive(onPduReceiveCallback pduReceived) {
    _callback = pduReceived;
}

void AgentuinoClass::freePdu(SNMP_PDU *pdu) {
    //
    memset(pdu->OID.data, 0, SNMP_MAX_OID_LEN);
    memset(pdu->VALUE.data, 0, SNMP_MAX_VALUE_LEN);
    free((char *) pdu);
}


// usage BER encoding
/*

TRAP PDU
The example TRAP above MIB file.
Community String is "public"
Agent's IPAddress is 192.168.1.128
Timestamp is 123456
The operMasterControl OID is in TRAP that has value is 1
  +---------+-----------+-------------------------------------------------------------------------------------------------------+-------------------+
  | SNMP    | Type      |                                           0x30                                                        |Sequence           |
  | message +-----------+-------------------------------------------------------------------------------------------------------+-------------------+
  |         | Length    |                                           0x82 0x00 0x44                                              |Length: 68         |
  |         +-----------+-------------------------------------------------------------------------------------------------------+-------------------+
  |         | Version   |                                           0x02                                                        |Integer            |
  |         |           |                                           0x01                                                        |Length: 1          |
  |         |           |                                           0x00                                                        |Value: 0           |
  |         +-----------+-------------------------------------------------------------------------------------------------------+-------------------+
  |         | Community |                                           0x04                                                        |Octet String       |
  |         |           |                                           0x06                                                        |Length: 6          |
  |         |           |                                           0x70 0x75 0x62 0x6C 0x69 0x63                               |Value: public      |
  |         +-----------+ -------+------------+---------------------------------------------------------------------------------+-------------------+
  |         | Data      | SNMPv1 | PDU type   |                     0xA4                                                        |TRAP PDU           |
  |         |           | PDU    +------------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | PDU length |                     0x82 0x00 0x35                                              |Length: 53         |
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | Enterprise |                     0x06                                                        |Object identifier  |
  |         |           |        | OID        |                     0x09                                                        |Length: 9          |
  |         |           |        |            |                     0x2B 0x06 0x01 0x04 0x01 0x82 0x99 0x5D 0x00                |Value:             |
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | Agent      |                     0x40                                                        |IpAddress          |
  |         |           |        | Address    |                     0x04                                                        |Length: 4          |
  |         |           |        |            |                     0xC0 0xA8 0x01 0x80                                         |Value:192.168.1.128|
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | Generic    |                     0x02                                                        |Integer            |
  |         |           |        | Trap Type  |                     0x01                                                        |Length: 1          |
  |         |           |        |            |                     0x06                                                        |Value: 6           |
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | Specific   |                     0x02                                                        |Integer            |
  |         |           |        | Trap       |                     0x01                                                        |Length: 1          |
  |         |           |        | Number     |                     0x01                                                        |Value: 1           |
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | Time       |                     0x43                                                        |TimeTicks          |
  |         |           |        | Stamp      |                     0x03                                                        |Length: 3          |
  |         |           |        |            |                     0x01 0xE2 0x40                                              |Value: 123456      |
  |         |           |        + -----------+---------------------------------------------------------------------------------+-------------------+
  |         |           |        | VarBind    |                     0x30                                                        |Sequence           |
  |         |           |        | List       +-----------+---------------------------------------------------------------------+-------------------+
  |         |           |        |            | Length    |         0x82 0x00 0x15                                              |Length: 21         |
  |         |           |        |            +-----------+---------------------------------------------------------------------+ ------------------+
  |         |           |        |            | VarBind 1 |         0x30                                                        |Sequence           |
  |         |           |        |            |           +-------+-------------------------------------------------------------+-------------------+
  |         |           |        |            |           | Len 1 | 0x82 0x00 0x11                                              |Length: 17         |
  |         |           |        |            |           +-------+-------------------------------------------------------------+-------------------+
  |         |           |        |            |           | OID 1 | 0x06                                                        |Object identifier  |
  |         |           |        |            |           |       | 0x0C                                                        |Length: 12         |
  |         |           |        |            |           |       | 0x2B 0x06 0x01 0x04 0x01 0x82 0x99 0x5D 0x03 0x01 0x01 0x01 |Value:             |
  |         |           |        |            |           +-------+-------------------------------------------------------------+-------------------+
  |         |           |        |            |           | Value | 0x02                                                        |Integer            |
  |         |           |        |            |           | 1     | 0x01                                                        |Length: 1          |
  |         |           |        |            |           |       | 0x01                                                        |Length: 1          |
  +---------+-----------+--------+------------+-----------+-------+-------------------------------------------------------------+-------------------+


  Data type identifier in SNMP
     Data type                 Identifier      Note
     Integer                      0x02         Primitive ASN.1 types
     Octet String                 0x04         Primitive ASN.1 types  
     Null                         0x05         Primitive ASN.1 types
     Object identifier            0x06         Primitive ASN.1 types
     Sequence                     0x30         Constructed ASN.1 types
     IpAddress                    0x40         Primitive SNMP application types
     Counter                      0x41         Primitive SNMP application types
     Gauge                        0x42         Primitive SNMP application types
     TimeTicks                    0x43         Primitive SNMP application types 
     Opaque                       0x44         Primitive SNMP application types
     NsapAddress                  0x45         Primitive SNMP application types
     GetRequest PDU               0xA0         Context-specific Constructed SNMP types
     GetNextRequest PDU           0xA1         Context-specific Constructed SNMP types
     GetResponse PDU              0xA2         Context-specific Constructed SNMP types
     SetRequest PDU               0xA3         Context-specific Constructed SNMP types
     Trap PDU                     0xA4         Context-specific Constructed SNMP types

http://www.rane.com/note161.html
http://www.opencircuits.com/SNMP_MIB_Implementation#Binary_File_Format_.3Cname.3E_trap.bin
http://wiki.tcl.tk/16631
 */

/**
 *
 * @info function for send SNMP TRAP
 * @param message
 * @param IP address recipient
 * @param boot time
 * @param enterprise OID
 * @param OID
 * @return void
 */
void AgentuinoClass::Trap(char Message[], byte RemIP[4], uint32_t Time, char enterprise_oid[], char oid_[]) {

    SNMP_OID oid;

    // trap = sequence, leng = pdu leng
    byte Type_and_leng[2] = {48, 0};

    // type = integer, leng = 1, value = 0 - snmp v1
    byte Version[3] = {2, 1, 0}; // Defined versión 1

    // type = octet string, leng = , value = comunity string
    byte Comunity_string[2 + strlen(_getCommName)];
    Comunity_string[0] = 4;
    Comunity_string[1] = strlen(_getCommName);

    for (uint8_t i = 2; i <= strlen(_getCommName) + 1; i++)
        Comunity_string[i] = (int) _getCommName[i - 2];

    // type = trap-pdu, 82, 0, leng = long as the other message
    byte PDU_type_and_leng[4] = {164, 130, 0, 53 + strlen(Message)};

    // type = identifier, leng = leng oid, value = oid
    size_t oid_size = 8;
    byte *enterprise_oid_dec = oid.fromString(enterprise_oid);

    byte OID[2 + oid_size];
    OID[0] = 6;
    OID[1] = oid_size;

    for (int i = 2; i < oid_size + 2; i++)
        OID[i] = enterprise_oid_dec[i - 2];

    // type = IP, leng = 4, value - IP sender
    byte IP_definition[2] = {64, 4};

    // type = integer, leng = 1, value = 6 - trap type
    byte Type_Trap[3] = {2, 1, 6};

    // type = 1, leng = 1, value = 1 - specific trap number
    byte extra_OID[3] = {2, 1, 1};

    // type = time stick, leng = 4
    byte Type_time_stick[2] = {67, 4};

    // The next part is to change the locUpTime into bytes
    int i = 0, k = 1, temp;
    byte suma = 0;
    uint32_t quotient;
    quotient = Time;
    byte hexadecimalNumber[4] = {0, 0, 0, 0};
    while (quotient != 0) {
        temp = quotient % 16;
        if (k == 1) {
            suma = temp;
            k = 2;
        } else {
            suma = suma + temp * 16;
            hexadecimalNumber[3 - i] = suma;
            i = i + 1;
            k = 1;
        }
        quotient = quotient / 16;
    }

    if (k == 2) {
        hexadecimalNumber[3 - i] = suma;
    }

    // type = sequence, 130, 0, leng =  long as the other message
    byte Var_Bind[4] = {48, 130, 0, 20 + strlen(Message)}; // Here is defined the size

    // type = sequence, 130, 0, leng = long as the other message
    byte Var_Bind1[4] = {48, 130, 0, 16 + strlen(Message)}; // Here is defined the size

    // type = intetifier, leng = leng oid, value = oid
    oid_size = 8;
    byte *oid_dec = oid.fromString(oid_);

    byte OID1[2 + oid_size];
    OID1[0] = 6;
    OID1[1] = oid_size;

    for (int i = 2; i < oid_size + 2; i++)
        OID1[i] = oid_dec[i - 2];

    // type = octetstring, leng = long as the other message, value = message
    byte Value1[2] = {4, strlen(Message)};

    Type_and_leng[1] = sizeof (Version) / sizeof (Version[0]) + sizeof (Comunity_string) / sizeof (Comunity_string[0])
            + sizeof (PDU_type_and_leng) / sizeof (PDU_type_and_leng[0]) + sizeof (OID) / sizeof (OID[0])
            + sizeof (IP_definition) / sizeof (IP_definition[0]) + sizeof (my_IP_address) / sizeof (my_IP_address[0])
            + sizeof (Type_Trap) / sizeof ( Type_Trap[0]) + sizeof (extra_OID) / sizeof ( extra_OID[0])
            + sizeof (Type_time_stick) / sizeof ( Type_time_stick[0]) + sizeof (hexadecimalNumber) / sizeof ( hexadecimalNumber[0])
            + sizeof (Var_Bind) / sizeof ( Var_Bind[0]) + sizeof (Var_Bind1) / sizeof ( Var_Bind1[0])
            + sizeof (OID1) / sizeof ( OID1[0]) + sizeof (Value1) / sizeof (Value1[0]) + strlen(Message);

    Var_Bind[3] = 6 + strlen(Message) + (sizeof (OID1) / sizeof ( OID1[0]));
    Var_Bind1[3] = 2 + strlen(Message) + (sizeof (OID1) / sizeof ( OID1[0]));

    PDU_type_and_leng[3] = sizeof (OID) / sizeof (OID[0])
            + sizeof (IP_definition) / sizeof (IP_definition[0]) + sizeof (my_IP_address) / sizeof (my_IP_address[0])
            + sizeof (Type_Trap) / sizeof ( Type_Trap[0]) + sizeof (extra_OID) / sizeof ( extra_OID[0])
            + sizeof (Type_time_stick) / sizeof ( Type_time_stick[0]) + sizeof (hexadecimalNumber) / sizeof ( hexadecimalNumber[0])
            + sizeof (Var_Bind) / sizeof ( Var_Bind[0]) + sizeof (Var_Bind1) / sizeof ( Var_Bind1[0])
            + sizeof (OID1) / sizeof ( OID1[0]) + sizeof (Value1) / sizeof (Value1[0]) + strlen(Message);

     
    Udp.beginPacket(RemIP, 162); // Here is defined the UDP port 162 to send the trap
    Udp.write(Type_and_leng, 2);
    Udp.write(Version, 3);
    Udp.write(Comunity_string, 2 + strlen(_getCommName));
    Udp.write(PDU_type_and_leng, 4);
    Udp.write(OID, sizeof (OID) / sizeof (OID[0]));
    Udp.write(IP_definition, 2);
    Udp.write(my_IP_address, 4);
    Udp.write(Type_Trap, 3);
    Udp.write(extra_OID, 3);
    Udp.write(Type_time_stick, 2);
    Udp.write(hexadecimalNumber, 4);
    Udp.write(Var_Bind, 4);
    Udp.write(Var_Bind1, 4);
    Udp.write(OID1, sizeof (OID1) / sizeof (OID1[0]));
    Udp.write(Value1, 2);
    Udp.write(Message, strlen(Message));
    Udp.endPacket();
}

// Create one global object
AgentuinoClass Agentuino;

// function for pduReceived() function
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;
char oid[SNMP_MAX_OID_LEN];
uint32_t prevMillis = 0;

/**
 *
 * @info function the perform at SNMP Requirements
 * @param none
 * @return void
 */
void pduReceived() {
    SNMP_PDU pdu;

    
    //
#ifdef DEBUG
    
#endif
    //
    api_status = Agentuino.requestPdu(&pdu);
    //
    if (pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET
            && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS) {
        //
        pdu.OID.toString(oid);

     

        if ( pdu.type == SNMP_PDU_GET_NEXT ) {
         
      char tmpOIDfs[SNMP_MAX_OID_LEN];
      if ( strcmp_P( oid, sysName ) == 0 ) {  
        strcpy_P ( oid, sysLocation ); 
        strcpy_P ( tmpOIDfs, sysLocation );        
        pdu.OID.fromString(tmpOIDfs);  
      } else if ( strcmp_P(oid, sysLocation ) == 0 ) {  
        strcpy_P ( oid, ipAddress );  
        strcpy_P ( tmpOIDfs, ipAddress );        
        pdu.OID.fromString(tmpOIDfs);          
      } 
      
      else if ( strcmp_P(oid, ipAddress ) == 0 ) {  
        strcpy_P ( oid, sysUpTime );  
        strcpy_P ( tmpOIDfs, sysUpTime );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {  
        strcpy_P ( oid, sysContact );  
        strcpy_P ( tmpOIDfs, sysContact );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp_P(oid, sysContact ) == 0 ) {  
        strcpy_P ( oid, telef );  
        strcpy_P ( tmpOIDfs, telef );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp_P(oid, telef ) == 0 ) {  
        strcpy_P ( oid, mail );  
        strcpy_P ( tmpOIDfs, mail );        
        pdu.OID.fromString(tmpOIDfs);

            
      } else if ( strcmp_P(oid, mail ) == 0 ) {  
        strcpy_P ( oid, ifIndex1 );  
        strcpy_P ( tmpOIDfs, ifIndex1 );        
        pdu.OID.fromString(tmpOIDfs);    
      }else if ( strcmp_P(oid, ifIndex1 ) == 0 ) {  
        strcpy_P ( oid, ifIndex2 );  
        strcpy_P ( tmpOIDfs, ifIndex2 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, ifIndex2 ) == 0 ) {  
        strcpy_P ( oid, ifIndex3 );  
        strcpy_P ( tmpOIDfs, ifIndex3 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, ifIndex3 ) == 0 ) {  
        
        strcpy_P ( oid, ifIndex4 );  
        strcpy_P ( tmpOIDfs, ifIndex4 );        
        pdu.OID.fromString(tmpOIDfs);  
      
      }else if ( strcmp_P(oid, ifIndex4 ) == 0 ) {  
         strcpy_P ( oid, des1 );  
        strcpy_P ( tmpOIDfs, des1 );        
        pdu.OID.fromString(tmpOIDfs);


        }else if ( strcmp_P(oid, des1 ) == 0 ) {  
        strcpy_P ( oid, des2 );  
        strcpy_P ( tmpOIDfs, des2 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, des2 ) == 0 ) {  
        strcpy_P ( oid, des3 );  
        strcpy_P ( tmpOIDfs, des3 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, des3 ) == 0 ) {  
        
        strcpy_P ( oid, des4 );  
        strcpy_P ( tmpOIDfs, des4 );        
        pdu.OID.fromString(tmpOIDfs);  
      
      }else if ( strcmp_P(oid, des4 ) == 0 ) {  
         strcpy_P ( oid, sensorTemp );  
        strcpy_P ( tmpOIDfs, sensorTemp );        
        pdu.OID.fromString(tmpOIDfs);


         }else if ( strcmp_P(oid, sensorTemp ) == 0 ) {  
        strcpy_P ( oid, sensorHum );  
        strcpy_P ( tmpOIDfs, sensorHum );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, sensorHum ) == 0 ) {  
        strcpy_P ( oid, led1 );  
        strcpy_P ( tmpOIDfs, led1 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, led1 ) == 0 ) {  
        
        strcpy_P ( oid, tempArd );  
        strcpy_P ( tmpOIDfs, tempArd );        
        pdu.OID.fromString(tmpOIDfs);  
      
      }else if ( strcmp_P(oid, tempArd ) == 0 ) {  
         strcpy_P ( oid, inst1 );  
        strcpy_P ( tmpOIDfs, inst1 );        
        pdu.OID.fromString(tmpOIDfs);


           }else if ( strcmp_P(oid, inst1 ) == 0 ) {  
        strcpy_P ( oid, inst2 );  
        strcpy_P ( tmpOIDfs, inst2 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, inst2 ) == 0 ) {  
        strcpy_P ( oid, inst3 );  
        strcpy_P ( tmpOIDfs, inst3 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, inst3 ) == 0 ) {  
        
        strcpy_P ( oid, inst4 );  
        strcpy_P ( tmpOIDfs, inst4 );        
        pdu.OID.fromString(tmpOIDfs);  
      
      }else if ( strcmp_P(oid, inst4 ) == 0 ) {  
         strcpy_P ( oid, tMuestreo1 );  
        strcpy_P ( tmpOIDfs, tMuestreo1 );        
        pdu.OID.fromString(tmpOIDfs);


        
           }else if ( strcmp_P(oid, tMuestreo1 ) == 0 ) {  
        strcpy_P ( oid, tMuestreo2 );  
        strcpy_P ( tmpOIDfs, tMuestreo2 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, tMuestreo2 ) == 0 ) {  
        strcpy_P ( oid, tMuestreo3 );  
        strcpy_P ( tmpOIDfs, tMuestreo3 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, tMuestreo3 ) == 0 ) {  
        
        strcpy_P ( oid, tMuestreo4 );  
        strcpy_P ( tmpOIDfs, tMuestreo4 );        
        pdu.OID.fromString(tmpOIDfs);  


       }else if ( strcmp_P(oid, tMuestreo4 ) == 0 ) {  
        strcpy_P ( oid, Index1 );  
        strcpy_P ( tmpOIDfs, Index1 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, Index1 ) == 0 ) {  
        strcpy_P ( oid, Index2 );  
        strcpy_P ( tmpOIDfs, Index2 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, Index2 ) == 0 ) {  
        
        strcpy_P ( oid, Index3 );  
        strcpy_P ( tmpOIDfs, Index3 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, Index3 ) == 0 ) {  
        
        strcpy_P ( oid, Index4 );  
        strcpy_P ( tmpOIDfs, Index4 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, Index4 ) == 0 ) {  
        
        strcpy_P ( oid, Index5 );  
        strcpy_P ( tmpOIDfs, Index5 );        
        pdu.OID.fromString(tmpOIDfs); 


         }else if ( strcmp_P(oid, Index5 ) == 0 ) {  
        strcpy_P ( oid, s1 );  
        strcpy_P ( tmpOIDfs, s1 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, s1 ) == 0 ) {  
        strcpy_P ( oid, s2 );  
        strcpy_P ( tmpOIDfs, s2 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, s2 ) == 0 ) {  
        
        strcpy_P ( oid, s3 );  
        strcpy_P ( tmpOIDfs, s3 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, s3 ) == 0 ) {  
        
        strcpy_P ( oid, s4 );  
        strcpy_P ( tmpOIDfs, s4 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, s4 ) == 0 ) {  
        
        strcpy_P ( oid, s5 );  
        strcpy_P ( tmpOIDfs, s5 );        
        pdu.OID.fromString(tmpOIDfs); 




             }else if ( strcmp_P(oid, s5 ) == 0 ) {  
        strcpy_P ( oid, dd1 );  
        strcpy_P ( tmpOIDfs, dd1 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, dd1 ) == 0 ) {  
        strcpy_P ( oid, dd2 );  
        strcpy_P ( tmpOIDfs, dd2 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, dd2 ) == 0 ) {  
        strcpy_P ( oid, dd3 );  
        strcpy_P ( tmpOIDfs, dd3 );        
        pdu.OID.fromString(tmpOIDfs);   
        }else if ( strcmp_P(oid, dd3 ) == 0 ) {  
        strcpy_P ( oid, dd4 );  
        strcpy_P ( tmpOIDfs, dd4 );        
        pdu.OID.fromString(tmpOIDfs);   
        }else if ( strcmp_P(oid, dd4 ) == 0 ) {  
        strcpy_P ( oid, dd5 );  
        strcpy_P ( tmpOIDfs, dd5 );        
        pdu.OID.fromString(tmpOIDfs);   

        }else if ( strcmp_P(oid, dd5 ) == 0 ) {  
        strcpy_P ( oid, us1 );  
        strcpy_P ( tmpOIDfs, us1 );        
        pdu.OID.fromString(tmpOIDfs);   

        
      }else if ( strcmp_P(oid, us1 ) == 0 ) {  
        strcpy_P ( oid, us2 );  
        strcpy_P ( tmpOIDfs, us2 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, us2 ) == 0 ) {  
        
        strcpy_P ( oid, us3 );  
        strcpy_P ( tmpOIDfs, us3 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, us3 ) == 0 ) {  
        
        strcpy_P ( oid, us4 );  
        strcpy_P ( tmpOIDfs, us4 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, us4 ) == 0 ) {  
        
        strcpy_P ( oid, us5 );  
        strcpy_P ( tmpOIDfs, us5 );        
        pdu.OID.fromString(tmpOIDfs); 


        }else if ( strcmp_P(oid, us5 ) == 0 ) {  
        strcpy_P ( oid, is1 );  
        strcpy_P ( tmpOIDfs, is1 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, is1 ) == 0 ) {  
        strcpy_P ( oid, is2 );  
        strcpy_P ( tmpOIDfs, is2 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, is2 ) == 0 ) {  
        
        strcpy_P ( oid, is3 );  
        strcpy_P ( tmpOIDfs, is3 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, is3 ) == 0 ) {  
        
        strcpy_P ( oid, is4 );  
        strcpy_P ( tmpOIDfs, is4 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, is4 ) == 0 ) {  
        
        strcpy_P ( oid, is5 );  
        strcpy_P ( tmpOIDfs, is5 );        
        pdu.OID.fromString(tmpOIDfs); 



        }else if ( strcmp_P(oid, is5 ) == 0 ) {  
        strcpy_P ( oid, IP1 );  
        strcpy_P ( tmpOIDfs, IP1 );        
        pdu.OID.fromString(tmpOIDfs);   
      }else if ( strcmp_P(oid, IP1 ) == 0 ) {  
        strcpy_P ( oid, IP2 );  
        strcpy_P ( tmpOIDfs, IP2 );        
        pdu.OID.fromString(tmpOIDfs);  
      }else if ( strcmp_P(oid, IP2 ) == 0 ) {  
        
        strcpy_P ( oid, IP3 );  
        strcpy_P ( tmpOIDfs, IP3 );        
        pdu.OID.fromString(tmpOIDfs);
        }else if ( strcmp_P(oid, IP3 ) == 0 ) {  
        
        strcpy_P ( oid, IP4 );  
        strcpy_P ( tmpOIDfs, IP4 );        
        pdu.OID.fromString(tmpOIDfs); 
        }else if ( strcmp_P(oid, IP4 ) == 0 ) {  
        
        strcpy_P ( oid, IP5 );  
        strcpy_P ( tmpOIDfs, IP5 );        
        pdu.OID.fromString(tmpOIDfs); 


       }else if ( strcmp_P(oid, IP5 ) == 0 ) {  
        strcpy_P ( oid, inventado );  
        strcpy_P ( tmpOIDfs, inventado );        
        pdu.OID.fromString(tmpOIDfs);
        



        
      
      }else if ( strcmp_P(oid, inventado ) == 0 ) {  
        strcpy_P ( oid, "1.0" );  
      } else {
          int ilen = strlen(oid);
          if ( strncmp_P(oid, sysName, ilen ) == 0 ) {
            strcpy_P ( oid, sysName ); 
            strcpy_P ( tmpOIDfs, sysName );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp_P(oid, sysLocation, ilen ) == 0 ) {
            strcpy_P ( oid, sysLocation ); 
            strcpy_P ( tmpOIDfs, sysLocation );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp_P(oid, ipAddress, ilen ) == 0 ) {
            strcpy_P ( oid, ipAddress ); 
            strcpy_P ( tmpOIDfs, ipAddress );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp_P(oid, sysUpTime, ilen ) == 0 ) {
            strcpy_P ( oid, sysUpTime ); 
            strcpy_P ( tmpOIDfs, sysUpTime );        
            pdu.OID.fromString(tmpOIDfs);   
          } else if ( strncmp_P(oid, sysContact, ilen ) == 0 ) {
            strcpy_P ( oid, sysContact ); 
            strcpy_P ( tmpOIDfs, sysContact );        
            pdu.OID.fromString(tmpOIDfs);    
            } else if ( strncmp_P(oid, telef, ilen ) == 0 ) {
            strcpy_P ( oid, telef ); 
            strcpy_P ( tmpOIDfs, telef );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, mail, ilen ) == 0 ) {
            strcpy_P ( oid, mail ); 
            strcpy_P ( tmpOIDfs, mail );        
            pdu.OID.fromString(tmpOIDfs);  
            }  
             
            else if ( strncmp_P(oid, ifIndex1, ilen ) == 0 ) {
            strcpy_P ( oid, ifIndex1 ); 
            strcpy_P ( tmpOIDfs, ifIndex1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, ifIndex2, ilen ) == 0 ) {
            strcpy_P ( oid, ifIndex2 ); 
            strcpy_P ( tmpOIDfs, ifIndex2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, ifIndex3, ilen ) == 0 ) {
            strcpy_P ( oid, ifIndex3 ); 
            strcpy_P ( tmpOIDfs, ifIndex3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, ifIndex4, ilen ) == 0 ) {
              
            strcpy_P ( oid, ifIndex4 ); 
            strcpy_P ( tmpOIDfs, ifIndex4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            




             else if ( strncmp_P(oid, des1, ilen ) == 0 ) {
            strcpy_P ( oid, des1 ); 
            strcpy_P ( tmpOIDfs, des1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, des2, ilen ) == 0 ) {
            strcpy_P ( oid, des2 ); 
            strcpy_P ( tmpOIDfs, des2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, des3, ilen ) == 0 ) {
            strcpy_P ( oid, des3 ); 
            strcpy_P ( tmpOIDfs, des3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, des4, ilen ) == 0 ) {
              
            strcpy_P ( oid, des4 ); 
            strcpy_P ( tmpOIDfs, des4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
          





             else if ( strncmp_P(oid, sensorTemp, ilen ) == 0 ) {
            strcpy_P ( oid, sensorTemp ); 
            strcpy_P ( tmpOIDfs, sensorTemp );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, sensorHum, ilen ) == 0 ) {
            strcpy_P ( oid, sensorHum ); 
            strcpy_P ( tmpOIDfs, sensorHum );        
            pdu.OID.fromString(tmpOIDfs);  
            }
           
            else if ( strncmp_P(oid, led1, ilen ) == 0 ) {
              
            strcpy_P ( oid, led1 ); 
            strcpy_P ( tmpOIDfs, led1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, tempArd, ilen ) == 0 ) {
                
            strcpy_P ( oid, tempArd ); 
           strcpy_P ( tmpOIDfs, tempArd );        
            pdu.OID.fromString(tmpOIDfs);  
            }




             else if ( strncmp_P(oid, inst1, ilen ) == 0 ) {
            strcpy_P ( oid, inst1 ); 
            strcpy_P ( tmpOIDfs, inst1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, inst2, ilen ) == 0 ) {
            strcpy_P ( oid, inst2 ); 
            strcpy_P ( tmpOIDfs, inst2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, inst3, ilen ) == 0 ) {
            strcpy_P ( oid, inst3 ); 
            strcpy_P ( tmpOIDfs, inst3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, inst4, ilen ) == 0 ) {
             
            strcpy_P ( oid, inst4 ); 
            strcpy_P ( tmpOIDfs, inst4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
           



             else if ( strncmp_P(oid, tMuestreo1, ilen ) == 0 ) {
            strcpy_P ( oid, tMuestreo1 ); 
            strcpy_P ( tmpOIDfs, tMuestreo1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, tMuestreo2, ilen ) == 0 ) {
            strcpy_P ( oid, tMuestreo2 ); 
            strcpy_P ( tmpOIDfs, tMuestreo2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, tMuestreo3, ilen ) == 0 ) {
            strcpy_P ( oid, tMuestreo3 ); 
            strcpy_P ( tmpOIDfs, tMuestreo3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, tMuestreo4, ilen ) == 0 ) {
              
            strcpy_P ( oid, tMuestreo4 ); 
            strcpy_P ( tmpOIDfs, tMuestreo4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }





            else if ( strncmp_P(oid, Index1, ilen ) == 0 ) {
            strcpy_P ( oid, Index1 ); 
            strcpy_P ( tmpOIDfs, Index1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, Index2, ilen ) == 0 ) {
            strcpy_P ( oid, Index2 ); 
            strcpy_P ( tmpOIDfs, Index2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, Index3, ilen ) == 0 ) {
            strcpy_P ( oid, Index3 ); 
            strcpy_P ( tmpOIDfs, Index3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, Index4, ilen ) == 0 ) {
             
            strcpy_P ( oid, Index4 ); 
            strcpy_P ( tmpOIDfs, Index4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, Index5, ilen ) == 0 ) {
              
            strcpy_P ( oid, Index5 ); 
            strcpy_P ( tmpOIDfs, Index5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }



              else if ( strncmp_P(oid, s1, ilen ) == 0 ) {
            strcpy_P ( oid, s1 ); 
            strcpy_P ( tmpOIDfs, s1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, s2, ilen ) == 0 ) {
            strcpy_P ( oid, s2 ); 
            strcpy_P ( tmpOIDfs, s2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, s3, ilen ) == 0 ) {
            strcpy_P ( oid, s3 ); 
            strcpy_P ( tmpOIDfs, s3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, s4, ilen ) == 0 ) {
              
            strcpy_P ( oid, s4 ); 
            strcpy_P ( tmpOIDfs, s4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, s5, ilen ) == 0 ) {
              
            strcpy_P ( oid, s5 ); 
            strcpy_P ( tmpOIDfs, s5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }



            else if ( strncmp_P(oid, dd1, ilen ) == 0 ) {
            strcpy_P ( oid, dd1 ); 
            strcpy_P ( tmpOIDfs, dd1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, dd2, ilen ) == 0 ) {
            strcpy_P ( oid, dd2 ); 
            strcpy_P ( tmpOIDfs, dd2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, dd3, ilen ) == 0 ) {
            strcpy_P ( oid, dd3 ); 
            strcpy_P ( tmpOIDfs, dd3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, dd4, ilen ) == 0 ) {
              
            strcpy_P ( oid, dd4 ); 
            strcpy_P ( tmpOIDfs, dd4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, dd5, ilen ) == 0 ) {
              
            strcpy_P ( oid, dd5 ); 
            strcpy_P ( tmpOIDfs, dd5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }



               else if ( strncmp_P(oid, us1, ilen ) == 0 ) {
            strcpy_P ( oid, us1 ); 
            strcpy_P ( tmpOIDfs, us1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid,us2, ilen ) == 0 ) {
            strcpy_P ( oid, us2 ); 
            strcpy_P ( tmpOIDfs, us2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, us3, ilen ) == 0 ) {
            strcpy_P ( oid, us3 ); 
            strcpy_P ( tmpOIDfs, us3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, us4, ilen ) == 0 ) {
              
            strcpy_P ( oid, us4 ); 
            strcpy_P ( tmpOIDfs, us4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, us5, ilen ) == 0 ) {
           
            strcpy_P ( oid, us5 ); 
            strcpy_P ( tmpOIDfs, us5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }




               else if ( strncmp_P(oid, is1, ilen ) == 0 ) {
            strcpy_P ( oid, is1 ); 
            strcpy_P ( tmpOIDfs, is1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, is2, ilen ) == 0 ) {
            strcpy_P ( oid, is2 ); 
            strcpy_P ( tmpOIDfs, is2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, is3, ilen ) == 0 ) {
            strcpy_P ( oid, is3 ); 
            strcpy_P ( tmpOIDfs, is3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, is4, ilen ) == 0 ) {
             
            strcpy_P ( oid, is4 ); 
            strcpy_P ( tmpOIDfs, is4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, is5, ilen ) == 0 ) {
              
            strcpy_P ( oid, is5 ); 
            strcpy_P ( tmpOIDfs, is5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }




               else if ( strncmp_P(oid, IP1, ilen ) == 0 ) {
            strcpy_P ( oid, IP1 ); 
            strcpy_P ( tmpOIDfs, IP1 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, IP2, ilen ) == 0 ) {
            strcpy_P ( oid, IP2 ); 
            strcpy_P ( tmpOIDfs, IP2 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, IP3, ilen ) == 0 ) {
            strcpy_P ( oid, IP3 ); 
            strcpy_P ( tmpOIDfs, IP3 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, IP4, ilen ) == 0 ) {
             
            strcpy_P ( oid, IP4 ); 
            strcpy_P ( tmpOIDfs, IP4 );        
            pdu.OID.fromString(tmpOIDfs);  
            }
            else if ( strncmp_P(oid, IP5, ilen ) == 0 ) {
             
            strcpy_P ( oid, IP5 ); 
            strcpy_P ( tmpOIDfs, IP5 );        
            pdu.OID.fromString(tmpOIDfs);  
            }

             else if ( strncmp_P(oid, inventado, ilen ) == 0 ) {
             
            strcpy_P ( oid, inventado ); 
            strcpy_P ( tmpOIDfs, inventado );        
            pdu.OID.fromString(tmpOIDfs);  
            }
          
      } 
    }
        //
        //Serial << "OID: " << oid << endl;
        //

        if (strcmp_P(oid, sysUpTime) == 0) {
            // handle sysName (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locUpTime
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
            
         
#endif
        } else if (strcmp_P(oid, sysName) == 0) {
            // handle sysName (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(locName, strlen(locName));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locName
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
        } else if (strcmp_P(oid, sysContact) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                
                status = pdu.VALUE.decode(locContact, strlen(locContact));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif   
                } else if (strcmp_P(oid, telef) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                
                status = pdu.VALUE.decode(phonenumber, strlen(phonenumber));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, phonenumber);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
         ;
#endif  
        } else if (strcmp_P(oid, mail) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
             
                status = pdu.VALUE.decode(email, strlen(email));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, email);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif   

        } else if (strcmp_P(oid, ipAddress) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
            
                status = pdu.VALUE.decode(ip, strlen(ip));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, ip);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
          
#endif
        } else if (strcmp_P(oid, sysLocation) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(locLocation, strlen(locLocation));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
        
        } else if (strcmp_P(oid, ifIndex1) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, index1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif 

        } else if (strcmp_P(oid, ifIndex2) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, index2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif 

        } else if (strcmp_P(oid, ifIndex3) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, index3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif 

        } else if (strcmp_P(oid, ifIndex4) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, index4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif 

        } else if (strcmp_P(oid, des1) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(descr1, strlen(descr1));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, descr1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif

        } else if (strcmp_P(oid, des2) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(descr2, strlen(descr2));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, descr2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        } else if (strcmp_P(oid, des3) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(descr3, strlen(descr3));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, descr3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        } else if (strcmp_P(oid, des4) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(descr4, strlen(descr4));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, descr4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
        } else if (strcmp_P(oid,sensorTemp ) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, sensortemp);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif 
        } 
        else if (strcmp_P(oid,sensorHum ) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, sensorhum);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif 
        }  else if (strcmp_P(oid, led1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(valorled1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, valorled1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif

            }  else if (strcmp_P(oid, tempArd) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(tempArduino);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tempArduino);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif

       }  else if (strcmp_P(oid, tMuestreo1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode3(&tiempoMuestreo1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
             
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tiempoMuestreo1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
         }  else if (strcmp_P(oid, tMuestreo2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&tiempoMuestreo2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
              
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tiempoMuestreo2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, tMuestreo3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&tiempoMuestreo3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tiempoMuestreo3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
          
#endif
        
         }      else if (strcmp_P(oid, tMuestreo4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&tiempoMuestreo4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, tiempoMuestreo4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
         


         }  else if (strcmp_P(oid, inst1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, instanteLectura1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, inst2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(instanteLectura2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, instanteLectura2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
         }  else if (strcmp_P(oid, inst3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(instanteLectura3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, instanteLectura3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            ;
#endif
        
         }      else if (strcmp_P(oid, inst4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(instanteLectura4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, instanteLectura4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
         }



















          else if (strcmp_P(oid, Index1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(ind1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ind1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, Index2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(ind2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ind2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
         }  else if (strcmp_P(oid, Index3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode1(ind3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ind3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
         }      else if (strcmp_P(oid, Index4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(ind4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ind4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif


          }      else if (strcmp_P(oid, Index5) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(ind5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ind5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif





          }



           else if (strcmp_P(oid, s1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&Sind1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, Sind1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, s2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&Sind2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, Sind2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, s3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&Sind3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, Sind3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
        
         }      else if (strcmp_P(oid, s4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&Sind4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, Sind4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif


          }      else if (strcmp_P(oid, s5) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&Sind5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
              
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, Sind5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif


          

          } else if (strcmp_P(oid, dd1) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(d1, strlen(d1));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, d1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
          

          } else if (strcmp_P(oid, dd2) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(d2, strlen(d2));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, d2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
          }

          else if (strcmp_P(oid, dd3) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(d3, strlen(d3));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, d3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
          }

          else if (strcmp_P(oid, dd4) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(d4, strlen(d4));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, d4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
          }

          else if (strcmp_P(oid, dd5) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(d5, strlen(d5));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, d5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
          }



          else if (strcmp_P(oid, us1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uS1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
             
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uS1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
#endif
            

         }  else if (strcmp_P(oid, us2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uS2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uS2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif
         }  else if (strcmp_P(oid, us3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uS3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uS3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
         }      else if (strcmp_P(oid, us4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uS4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uS4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif


          }      else if (strcmp_P(oid, us5) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uS5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uS5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif



          }


          else if (strcmp_P(oid, is1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uI1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uI1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, is2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uI2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uI2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
         }  else if (strcmp_P(oid, is3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uI3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
               
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uI3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif
        
         }      else if (strcmp_P(oid, is4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uI4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uI4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif


          }      else if (strcmp_P(oid, is5) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode2(&uI5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, uI5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif













          } else if (strcmp_P(oid, IP1) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                
                status = pdu.VALUE.decodeIP(ip1);
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip1);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip1);
                
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
         
#endif



            } else if (strcmp_P(oid, IP2) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
               
                status = pdu.VALUE.decodeIP(ip2);
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip2);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif



           } else if (strcmp_P(oid, IP3) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
           
                status = pdu.VALUE.decodeIP(ip3);
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip3);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif


            } else if (strcmp_P(oid, IP4) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
            
                status = pdu.VALUE.decodeIP(ip4);
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip4);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif





          } else if (strcmp_P(oid, IP5) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
           
                status = pdu.VALUE.decodeIP(ip5);
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip5);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
           
#endif

          } else if (strcmp_P(oid, inventado) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, "FINALMIB");
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
            //
#ifdef DEBUG
            
#endif

        }
         

           else {
            // oid does not exist
            //
            // response packet - object not found
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = SNMP_ERR_NO_SUCH_NAME;
        
           }
        //
        Agentuino.responsePdu(&pdu);
    }
    //
    Agentuino.freePdu(&pdu);
    //
}
