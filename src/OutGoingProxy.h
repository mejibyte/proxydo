class OutgoingProxy{
private:
	int port;
	vector<string> bloquedHost;
public:
	void run();
	void DestinationThread(ServerSocket,vector<string>);
	
}