#include "incglobal.h"

using namespace std;

Logger LOG = Logger("client.log");

int main(int argc, char* argv[]) {
	UdpSocket broadcast;
	
	// give socket permission / option to send data over broadcast address
	unsigned long broadcastOpt = 1;
	if (!broadcast.setSockOpt(SOL_SOCKET, SO_BROADCAST, broadcastOpt)) {
		Socket::outputSocketLastError();
		return 0;
	}

	broadcast.bind(InetHost((unsigned long)INADDR_ANY, BROADCAST_PORT));

	LOG.logInformation("Searching For Client");

	InetHost clientAddr;

	while (true) {
		char buffer[13];
		if (broadcast.recvFrom(buffer, 13, &clientAddr) == 13 && !memcmp(buffer, "RobotTime(TM)", 13)) {
			LOG.logSuccess("Found Client, Connecting...");
			break;
		}
	}

	clientAddr.setPort(CONNECT_PORT);
	TcpClient client(clientAddr);

	if (!client.connect()) {
		LOG.logError(
			"Unable To Connect To Client %s:%u -> %s",
			inet_ntoa(client.getSockName().getAddress()),
			client.getSockName().getPort(),
			Socket::getSocketLastError().c_str()
		);
		return 0;
	}
	else {
		LOG.logSuccess("Connected");
	}

	FrameHeader hdr;

	if (client.recv((char*)&hdr, sizeof(FrameHeader)) < 1) {
		LOG.logError(
			"Unable To Receive Frame Header Info From Client -> %s",
			Socket::getSocketLastError().c_str()
		);
		return 0;
	}


	cv::namedWindow("Live", CV_WINDOW_AUTOSIZE);
	size_t frameSz = hdr.frameRows * hdr.frameCols * hdr.frameDataTypeSz;
	std::vector<uchar> frameData(frameSz);

	LOG.logInformation("Press `q` To Quit");

	while (true) {
		if (kbhit() && getch() == 'q') {
			break;
		}

		for (int i = 0, bytes = 0; i < frameSz; i += bytes) {
			if ((bytes = client.recv((char*)(frameData.data() + i), frameSz - i, 0)) == -1) {
				Socket::outputSocketLastError();
				break;
			}
		}

		LOG.logInformation("Header says size is %i x %i", hdr.frameCols, hdr.frameRows);
		cv::Mat frame(cv::Size(hdr.frameCols, hdr.frameRows), hdr.frameDataType, frameData.data());

		cv::imshow("Live", frame);
		cv::waitKey(50);
	}
}