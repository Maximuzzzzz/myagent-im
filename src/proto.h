#ifndef MRIM_PROTO_H
#define MRIM_PROTO_H

#define PROTO_VERSION_MAJOR     1
#define PROTO_VERSION_MINOR     23
#define PROTO_VERSION ((((quint32)(PROTO_VERSION_MAJOR))<<16)|(quint32)(PROTO_VERSION_MINOR))

#define PROTO_MAJOR(p) (((p)&0xFFFF0000)>>16)
#define PROTO_MINOR(p) ((p)&0x0000FFFF)


typedef struct mrim_packet_header_t
{
	quint32	magic;		// Magic
	quint32	proto;		// Версия протокола
	quint32	seq;		// Sequence
	quint32	msg;		// Тип пакета
	quint32	dlen; 		// Длина данных
	quint32	from;		// Адрес отправителя
	quint32	fromport;	// Порт отправителя
	quint8	reserved[16];	// Зарезервировано
}
mrim_packet_header_t;

#define HEADER_SIZE 44

#define CS_MAGIC    0xDEADBEEF		// Клиентский Magic ( C <-> S )


// UNICODE = (UTF-16LE) (>=1.17)

/***************************************************************************

		ПРОТОКОЛ СВЯЗИ КЛИЕНТ-СЕРВЕР

 ***************************************************************************/

#define MRIM_CS_HELLO       	0x1001  // C -> S
	// empty

#define MRIM_CS_HELLO_ACK   	0x1002  // S -> C
	// mrim_connection_params_t


#define MRIM_CS_LOGIN_ACK   	0x1004  // S -> C
	// empty

#define MRIM_CS_LOGIN_REJ   	0x1005  // S -> C
	// LPS reason

#define MRIM_CS_PING        	0x1006  // C -> S
	// empty

#define MRIM_CS_MESSAGE			0x1008  // C -> S
	// UL flags
	// LPS to
	// LPS message
	// LPS rtf-formatted message (>=1.1)

	#define MESSAGE_FLAG_OFFLINE		0x00000001
	#define MESSAGE_FLAG_NORECV		0x00000004
	#define MESSAGE_FLAG_AUTHORIZE		0x00000008 	// X-MRIM-Flags: 00000008
	#define MESSAGE_FLAG_SYSTEM		0x00000040
	#define MESSAGE_FLAG_RTF		0x00000080
	#define MESSAGE_FLAG_CONTACT		0x00000200
	#define MESSAGE_FLAG_NOTIFY		0x00000400
	#define MESSAGE_FLAG_SMS		0x00000800
	#define MESSAGE_FLAG_MULTICAST		0x00001000
	#define MESSAGE_SMS_DELIVERY_REPORT	0x00002000
	#define MESSAGE_FLAG_ALARM		0x00004000
	#define MESSAGE_FLAG_FLASH		0x00008000
	#define MESSAGE_FLAG_SPAMF_SPAM		0x00020000	// ����� ����������� �� ���� - ������� ����� � ���� ������ ;������� ������������, �������� � ������ ��������� ��������� ��� �������� ������ ��������
	#define MESSAGE_FLAG_v1p16		0x00100000	// ��� ������������� �������
	#define MESSAGE_FLAG_CP1251		0x00200000
	#define MESSAGE_FLAG_CONFERENCE		0x00400000
// LPS to e-mail ANSI
// LPS message ANSI/UNICODE (see flags)
// LPS rtf-formatted message (>=1.1) ???

#define MAX_MULTICAST_RECIPIENTS 50
	#define MESSAGE_USERFLAGS_MASK		0x000036A8	// Flags that user is allowed to set himself


#define MRIM_CS_SMS	0x1039
// UL - unknown
// LPS - number
// LPS - text

#define MRIM_CS_SMS_ACK	0x1040
// UL - status
	#define SMS_STATUS_OK 1

#define MRIM_CS_MESSAGE_ACK		0x1009  // S -> C
	// UL msg_id
	// UL flags
	// LPS from
	// LPS message
	// LPS rtf-formatted message (>=1.1)




#define MRIM_CS_MESSAGE_RECV	0x1011	// C -> S
	// LPS from
	// UL msg_id

#define MRIM_CS_MESSAGE_STATUS	0x1012	// S -> C
	// UL status
	#define MESSAGE_DELIVERED		0x0000	// Message delivered directly to user
	#define MESSAGE_REJECTED_NOUSER		0x8001  // Message rejected - no such user
	#define MESSAGE_REJECTED_INTERR		0x8003	// Internal server error
	#define MESSAGE_REJECTED_LIMIT_EXCEEDED	0x8004	// Offline messages limit exceeded
	#define MESSAGE_REJECTED_TOO_LARGE	0x8005	// Message is too large
	#define	MESSAGE_REJECTED_DENY_OFFMSG	0x8006	// User does not accept offline messages

#define MRIM_CS_USER_STATUS	0x100F	// S -> C
	// UL status
	#define STATUS_OFFLINE		0x00000000
	#define STATUS_ONLINE		0x00000001
	#define STATUS_AWAY		0x00000002
	#define STATUS_UNDETERMINATED	0x00000003
	#define STATUS_OTHER_ONLINE		0x00000004
	#define STATUS_FLAG_INVISIBLE	0x80000000
	// LPS status_text (NULL if status == 0)
	// LPS status_descr
	// ??? //TODO (almost always == 0)
	// LPS user
	// ??? //TODO (almost always == 0xffffff)
	// client version
	

#define MRIM_CS_LOGOUT			0x1013	// S -> C
	// UL reason
	#define LOGOUT_NO_RELOGIN_FLAG	0x0010		// Logout due to double login

#define MRIM_CS_CONNECTION_PARAMS	0x1014	// S -> C
	// mrim_connection_params_t

#define MRIM_CS_USER_INFO			0x1015	// S -> C
	// (LPS key, LPS value)* X


#define MRIM_CS_ADD_CONTACT			0x1019	// C -> S
	// UL flags (group(2) or usual(0)
	// UL group id (unused if contact is group)
	// LPS contact
	// LPS name
	// LPS unused
	#define CONTACT_FLAG_REMOVED		0x00000001
	#define CONTACT_FLAG_GROUP		0x00000002
	#define CONTACT_FLAG_INVISIBLE		0x00000004
	#define CONTACT_FLAG_VISIBLE		0x00000008
	#define CONTACT_FLAG_IGNORE		0x00000010
	#define CONTACT_FLAG_SHADOW		0x00000020
	#define CONTACT_FLAG_PHONE		0x00100000

	#define SMS_CONTACT_GROUP		0x000f4342

#define MRIM_CS_ADD_CONTACT_ACK			0x101A	// S -> C
	// UL status
	// UL contact_id or (quint32)-1 if status is not OK

	#define CONTACT_OPER_SUCCESS		0x0000
	#define CONTACT_OPER_ERROR		0x0001
	#define CONTACT_OPER_INTERR		0x0002
	#define CONTACT_OPER_NO_SUCH_USER	0x0003
	#define CONTACT_OPER_INVALID_INFO	0x0004
	#define CONTACT_OPER_USER_EXISTS	0x0005
	#define CONTACT_OPER_GROUP_LIMIT	0x6

#define MRIM_CS_MODIFY_CONTACT			0x101B	// C -> S
	// UL id
	// UL flags - same as for MRIM_CS_ADD_CONTACT
	// UL group id (unused if contact is group)
	// LPS contact
	// LPS name
	// LPS unused

#define MRIM_CS_MODIFY_CONTACT_ACK		0x101C	// S -> C
	// UL status, same as for MRIM_CS_ADD_CONTACT_ACK

#define MRIM_CS_OFFLINE_MESSAGE_ACK		0x101D	// S -> C
	// UIDL
	// LPS offline message

#define MRIM_CS_DELETE_OFFLINE_MESSAGE		0x101E	// C -> S
	// UIDL


#define MRIM_CS_AUTHORIZE			0x1020	// C -> S
	// LPS user

#define MRIM_CS_AUTHORIZE_ACK			0x1021	// S -> C
	// LPS user

#define MRIM_CS_CHANGE_STATUS			0x1022	// C -> S
	// UL new status
	// LPS id status
	// LPS status description
	// UL ??
	// UL ??

#define MRIM_CS_GET_MPOP_SESSION		0x1024	// C -> S


#define MRIM_CS_MPOP_SESSION			0x1025	// S -> C
	#define MRIM_GET_SESSION_FAIL		0
	#define MRIM_GET_SESSION_SUCCESS	1
	//UL status
	// LPS mpop session


#define MRIM_CS_FILE_TRANSFER                   0x1026  // C->S
		//LPS TO/FROM
		//DWORD id_request - uniq per connect
		//DWORD FILESIZE
		//LPS:  //FILENAME:SIZE;FILENAME:SIZE
				//DESCRIPTION
				//IP:PORT,IP:PORT

#define MRIM_CS_FILE_TRANSFER_ACK               0x1027 // S->C
		//DWORD status
		#define FILE_TRANSFER_STATUS_OK                 1
		#define FILE_TRANSFER_STATUS_DECLINE            0
		#define FILE_TRANSFER_STATUS_ERROR              2
		#define FILE_TRANSFER_STATUS_INCOMPATIBLE_VERS  3
		#define FILE_TRANSFER_MIRROR                    4
		//LPS TO/FROM
		//DWORD id_request
		//LPS DESCRIPTION

//white pages!
#define MRIM_CS_WP_REQUEST			0x1029 //C->S
//DWORD field, LPS value
#define PARAMS_NUMBER_LIMIT			50
#define PARAM_VALUE_LENGTH_LIMIT		64

//if last symbol in value eq '*' it will be replaced by LIKE '%'
// params define
// must be  in consecutive order (0..N) to quick check in check_anketa_info_request
enum {
  MRIM_CS_WP_REQUEST_PARAM_USER		= 0,
  MRIM_CS_WP_REQUEST_PARAM_DOMAIN,
  MRIM_CS_WP_REQUEST_PARAM_NICKNAME,
  MRIM_CS_WP_REQUEST_PARAM_FIRSTNAME,
  MRIM_CS_WP_REQUEST_PARAM_LASTNAME,
  MRIM_CS_WP_REQUEST_PARAM_SEX	,
  MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY,
  MRIM_CS_WP_REQUEST_PARAM_DATE1	,
  MRIM_CS_WP_REQUEST_PARAM_DATE2	,
  //!!!!!!!!!!!!!!!!!!!online request param must be at end of request!!!!!!!!!!!!!!!
  MRIM_CS_WP_REQUEST_PARAM_ONLINE	,
  MRIM_CS_WP_REQUEST_PARAM_STATUS	,	 // we do not used it, yet
  MRIM_CS_WP_REQUEST_PARAM_CITY_ID,
  MRIM_CS_WP_REQUEST_PARAM_ZODIAC,
  MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_MONTH,
  MRIM_CS_WP_REQUEST_PARAM_BIRTHDAY_DAY,
  MRIM_CS_WP_REQUEST_PARAM_COUNTRY_ID,
  MRIM_CS_WP_REQUEST_PARAM_MAX
};

#define MRIM_CS_ANKETA_INFO			0x1028 //S->C
//DWORD status
	#define MRIM_ANKETA_INFO_STATUS_OK		1
	#define MRIM_ANKETA_INFO_STATUS_NOUSER		0
	#define MRIM_ANKETA_INFO_STATUS_DBERR		2
	#define MRIM_ANKETA_INFO_STATUS_RATELIMERR	3
//DWORD fields_num
//DWORD max_rows
//DWORD server_time sec since 1970 (unixtime)
// fields set 				//%fields_num == 0
//values set 				//%fields_num == 0
//LPS value (numbers too)


#define MRIM_CS_MAILBOX_STATUS			0x1033
//DWORD new messages in mailbox

#define MRIM_CS_NEW_MAIL			0x1048
//UL - unread messages number
//LPS - sender
//LPS - subject
//UL - unix_time
//UL - unknown

#define MRIM_CS_CONTACT_LIST2		0x1037 //S->C
// UL status
#define GET_CONTACTS_OK			0x0000
#define GET_CONTACTS_ERROR		0x0001
#define GET_CONTACTS_INTERR		0x0002
//DWORD status  - if ...OK than this staff:
//DWORD groups number
//mask symbols table:
//'s' - lps
//'u' - unsigned long
//'z' - zero terminated string
//LPS groups fields mask
//LPS contacts fields mask
//group fields
//contacts fields
//groups mask 'us' == flags, name
//contact mask 'uussuu' flags, flags, internal flags, status
	#define CONTACT_INTFLAG_NOT_AUTHORIZED	0x0001


//old packet cs_login with cs_statistic
#define MRIM_CS_LOGIN2       	0x1038  // C -> S
#define MAX_CLIENT_DESCRIPTION 256
// LPS login
// LPS password
// DWORD status
//+ statistic packet data:
// LPS client description //max 256



#define MRIM_CS_PROXY		0x1044
// LPS          to e-mail ANSI
// DWORD        id_request
// DWORD        data_type
	#define MRIM_PROXY_TYPE_VOICE	1
	#define MRIM_PROXY_TYPE_FILES	2
	#define MRIM_PROXY_TYPE_CALLOUT	3
// LPS          user_data ???
// LPS          lps_ip_port ???
// DWORD        session_id[4]

#define MRIM_CS_PROXY_ACK		0x1045
//DWORD         status
	#define PROXY_STATUS_OK					1
	#define PROXY_STATUS_DECLINE			0
	#define PROXY_STATUS_ERROR				2
	#define PROXY_STATUS_INCOMPATIBLE_VERS	3
	#define PROXY_STATUS_NOHARDWARE			4
	#define PROXY_STATUS_MIRROR				5
	#define PROXY_STATUS_CLOSED				6
// LPS           to e-mail ANSI
// DWORD         id_request
// DWORD         data_type
// LPS           user_data ???
// LPS:          lps_ip_port ???
// DWORD[4]      Session_id

#define MRIM_CS_PROXY_HELLO			0x1046
// DWORD[4]      Session_id

#define MRIM_CS_PROXY_HELLO_ACK		0x1047

#define MRIM_SC_MICROBLOG_TEXT			0x1063
// ??
// LPS email
// ??
// ??
// ??
// LPS text
// LPS xml version


#define MRIM_CS_MICROBLOG_TEXT			0x1064
// LPS status

#define MRIM_CS_LOGIN3				0x1078
// LPS login
// LPS password
// DWORD ??? = ffffffff
// LPS version
// locale
// DWORD ??? = 10000000
// DWORD ??? = 01000000
// LPS ??? = geo-list
// LPS version2
// for ;;
	// DWORD[2] id_argument
	// DWORD ??? = 00000002 || 00000001
	// DWORD data

typedef struct mrim_connection_params_t
{
	quint32	ping_period;
}
mrim_connection_params_t;

//} //namespace Proto

#endif // MRIM_PROTO_H
