#include "incglobal.h"

// kill me
Logger LOG = Logger("server.log");



static void broadcastTaskCallback(bool& flag, const TaskArgList& args) {
	// setup for UDP socket object and broadcast address
	UdpSocket broadcaster;
	InetHost broadcastAddr(ULONG_MAX, BROADCAST_PORT);


	// give socket permission / option to send data over broadcast address
	unsigned long broadcastOpt = 1;
	if (!broadcaster.setSockOpt(SOL_SOCKET, SO_BROADCAST, broadcastOpt)) {
		Socket::outputSocketLastError();
		return;
	}

	// broadcast the phrase `RobotTime(TM)` over `BROADCAST_PORT`
	while (flag) {
		broadcaster.sendTo("RobotTime(TM)", 13, &broadcastAddr);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	// close socket
	if (!broadcaster.close()) {
		Socket::outputSocketLastError();
	}
}

static void clientHandler(TcpClient& client) {

	cv::VideoCapture cam(0);
	cv::Mat camFrame;
	std::vector<uchar> camData;
	FrameHeader hdr;

	cam >> camFrame;

	LOG.logInformation("Feed has dimensions: %i x %i", camFrame.cols, camFrame.rows);
	hdr.frameCols = camFrame.cols;
	hdr.frameRows = camFrame.rows;
	hdr.frameDataType = camFrame.type();
	hdr.frameDataTypeSz = camFrame.elemSize();

	if (!client.send(
		(const char*)&hdr,
		sizeof(FrameHeader)
	)) {
		LOG.logError(
			"Unable To Send Stream Header To Client %s:%u -> %s", 
			inet_ntoa(client.getSockName().getAddress()), 
			client.getSockName().getPort(), 
			Socket::getSocketLastError().c_str()
		);
		return;
	}

	while (true) {
		cam >> camFrame;

		if (client.send(
			(const char*)camFrame.data,
			camFrame.total() * camFrame.elemSize()
		) != (camFrame.total() * camFrame.elemSize())) {
			LOG.logError(
				"Unable To Send Stream Frame Data To Client %s:%u -> %s",
				inet_ntoa(client.getSockName().getAddress()),
				client.getSockName().getPort(),
				Socket::getSocketLastError().c_str()
			);
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(41)); // shitty framerate limit at ~24fps
	}
}




int main(int argc, char* argv[]) {
	Task broadcastTask(broadcastTaskCallback);
	broadcastTask.start();


	// tcp protocol server listening over `CONNECT_PORT`
	TcpServer server(CONNECT_PORT);


	// check that server socket is initialized
	if (!server.isValid()) {
		LOG.logError("TCP Server Unable To Create -> %s", Socket::getSocketLastError().c_str());
		return 0;
	}
	else { LOG.logInformation("TCP Server Socket Created"); }


	// check that server socket is bound to address and port
	if (!server.bind()) {
		LOG.logError("TCP Server Unable To Bind -> %s", Socket::getSocketLastError().c_str());
		return 0;
	}
	else { LOG.logInformation("TCP Server Socket Bound To Local Address On Port %u", CONNECT_PORT); }


	// check that server socket is able to listen for clients
	if (!server.listen()) {
		LOG.logError("TCP Server Unable To Listen -> %s", Socket::getSocketLastError().c_str());
		return 0;
	}
	else { LOG.logInformation("TCP Server Socket Listening For Clients", CONNECT_PORT); }


	// handles clients, dont care enough about "good practices" to do it properly
	while (true) {
		TcpClient client = server.accept();
		if (client.isValid()) {
			LOG.logInformation("TCP Server Accepted Client %s:%u", inet_ntoa(client.getSockName().getAddress()), client.getSockName().getPort());
			clientHandler(client);
			client.close();
			LOG.logInformation("TCP Server Removed Client %s:%u", inet_ntoa(client.getSockName().getAddress()), client.getSockName().getPort());
		}
		else {
			int wouldBlock;
			#ifdef _WIN32
				wouldBlock = WSAEWOULDBLOCK;
			#else
				wouldBlock = EWOULDBLOCK;
			#endif
			if (socket_errno != wouldBlock) {
				LOG.logError("Tcp Server Socket Error -> ", Socket::getSocketLastError());
				break;
			}
		}
	}
}