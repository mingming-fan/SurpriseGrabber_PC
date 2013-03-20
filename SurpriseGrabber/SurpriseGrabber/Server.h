#ifndef SERVER_H
#define  SERVER_H

#include "btwlib.h"
#include "SerialComm.h"
class Server:CSppServer
{
	// Construction
public:
	Server::Server()
		: m_pBtIf(NULL),
		m_isConnected(FALSE),
		m_ComPort(0),
		m_isStarted(FALSE)
	{
		m_pBtIf = new CBtIf;
	}

	Server::~Server(){
		delete (m_pBtIf);
		m_pBtIf = NULL;
	}

public:
	void OnServerStateChange(BD_ADDR bda, DEV_CLASS dev_class, BD_NAME name, short com_port, SPP_STATE_CODE state)
	{
		if (state == SPP_CONNECTED)
		{
			if(!m_isConnected){
				m_ComPort = com_port;
				m_isConnected = TRUE;
			}
			if (serialComm.OpenCommPort((UINT8)m_ComPort))
			{
			//	root->AddLogText("-----------------------");
			}
			else
			{
				char* msg = new char[20];
				msg = "!!!!!!!";
				//exit(-1);
			//	root->AddLogText("~~~~~~~~~~~~~~");
			}
		}
		else{
			m_ComPort = 0;
			m_isConnected = FALSE;
			serialComm.CloseCommPort();
		}
	};
	void StartServer()
	{
		SPP_SERVER_RETURN_CODE port_rc;
		SPP_SERVER_RETURN_CODE test = CSppServer::SUCCESS;
		port_rc = CSppServer::CreateConnection(m_serviceName);
		if (CSppServer::SUCCESS != port_rc)
		{
			exit(-2);
		//	root->AddLogText("!!!!!!!!!!!!!!!!!!!!");
		}
		m_isStarted = TRUE;
	};
	CSerialComm serialComm;
	CBtIf *m_pBtIf; // instantiating this object initializes access to the stack server !!
	char m_serviceName[BT_MAX_SERVICE_NAME_LEN + 1];
	//CBlueComChatDlg *root;

	short m_ComPort;
	BOOL  m_isConnected;
	BOOL  m_isStarted;
};


#endif