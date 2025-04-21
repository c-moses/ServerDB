class QueuedRPC
{
	string header;
	string message;
	
	void CreateRPC(string newHeader, string newMessage)
	{
		header = newHeader;
		message = newMessage;
	}
}

class ServerRPCs
{
	protected static ref ServerRPCs s_Instance;
	protected ref map<string, ref QueuedRPC> m_rpcQueue;
	protected string m_rpcDirectory = "$profile:/ServerRPCs";
	protected ref array<string> m_types = {"GameState", "Events"};
	
	void AddRPC(string type, string header, string message = "")
	{
		if (type.IsEmpty() || header.IsEmpty())
			return;
		
		if (!m_types.Contains(type))
			return;
		
		if (type == "GameState")
		{
			SendRPC(m_rpcDirectory + "/" + type, header, message, true);
		}
		else
		{
			QueuedRPC rpc = new QueuedRPC();
			rpc.CreateRPC(header, message);
			m_rpcQueue.Insert(type, rpc);
		}
	}
	
	void ScheduledRPCsUpdate()
	{
		if (m_rpcQueue.Count() <= 0)
			return;
		
		foreach (string type, QueuedRPC rpc : m_rpcQueue)
		{
			string dirPath = m_rpcDirectory + "/" + type;
			FileIO.MakeDirectory(dirPath);
			
			SendRPC(dirPath, rpc.header, rpc.message);
		}
	
		m_rpcQueue.Clear();
	}
	
	void SendRPC(string filePath, string fileName, string contents, bool overwrite = false)
	{
		FileIO.MakeDirectory(filePath);
		
		string rpcPath;
		
		if (overwrite)
		{
			rpcPath = string.Format("%1/%2.txt", filePath, fileName);
		}
		else
		{
			int timestamp = System.GetUnixTime();
			rpcPath = string.Format("%1/%2_%3.txt", filePath, fileName, timestamp);
		}
		
		FileHandle handle = FileIO.OpenFile(rpcPath, FileMode.WRITE);
	
		if (handle)
		{
			handle.WriteLine(contents);
			handle.Close();
		}
	}
	
	void ValidateDirectory()
	{
		foreach (string type : m_types)
		{
			FileIO.MakeDirectory(m_rpcDirectory + "/" + type);
		}
	}
	
	void ServerRPCs()
	{
		ValidateDirectory();
		m_rpcQueue = new map<string, ref QueuedRPC>();
	}

	static ServerRPCs GetInstance()
	{
		if (!s_Instance)
			s_Instance = new ServerRPCs();
		return s_Instance;
	}
}