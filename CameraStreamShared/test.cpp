#include "incglobal.h"
#include "huffman.h"
#include "Byte.h"

using namespace std;
using namespace cv;

#define FRAME_CHANGE_PACKET_ID	1
#define FRAME_HEADER_PACKET_ID	2
#define FRAME_DATA_PACKET_ID	3

typedef struct {
	uint idx;
	unsigned long long val;
} FrameChangeDesc, *pFrameChangeDesc;

typedef struct _framechangepacket {
	uchar id = FRAME_CHANGE_PACKET_ID;
	uint changeCount;
	FrameChangeDesc changes[1];
} FrameChangePacket, *pFrameChangePacket;

typedef struct _framedata {
	uchar id = FRAME_DATA_PACKET_ID;
	uint dataLen;
	uchar data[1];
} FrameData, *pFrameData;

#define	ABS16(_s_)	(((_s_) & (unsigned short)(1 << 15)) ? (~(_s_) + 1) : (_s_))

static inline uchar frameDataGetLargestDivisibleSize(const std::vector<uchar>& packet) {
	switch(packet.size() & (size_t)7) {
	case 0:
		return 8;
	case 6:
	case 2:
		return 2;
	case 4:
		return 4;
	case 1:
	case 3:
	case 5:
	case 7:
		return 1;
	}
}

// only using smart pointers to stop newbs from fucking something up; I swear im not a C++ standard shill
// returns null if it's not advantageous to send change instead of whole frame, or `b` is larger than `a`
static std::shared_ptr<FrameChangePacket> buildChangePacket(
	const std::vector<uchar>& a,
	const std::vector<uchar>& b
) {
	if (b.size() < a.size()) {
		return NULL;
	}

	size_t changeCount = 0;

	for (size_t i = 0, step = 8; i != a.size(); i += step) {
		if (i + step > a.size() && i > 1) {
			step /= 2;
		}

		switch (step)
		{
		}
	}

	for (size_t i = 0; i != a.size(); i++) {
		short db = (short)a[i] - (short)b[i];
		if (ABS16(db) > 20) {
			changeCount++;
		}
	}

	size_t changePacketSize = sizeof(FrameChangePacket) + (changeCount * sizeof(FrameChangeDesc)) - sizeof(FrameChangeDesc);

	if (!changeCount || changePacketSize >= b.size()) {
		printf("Not Sending Change\n");
		return NULL;
	} else{
		printf("!!!! Sending Changee\n");
	}

	pFrameChangePacket changePacket = (pFrameChangePacket)malloc(sizeof(FrameChangePacket) + (changeCount * sizeof(FrameChangeDesc)) - sizeof(FrameChangeDesc));
	changePacket->id = FRAME_CHANGE_PACKET_ID;
	changePacket->changeCount = changeCount;

	cout << "Change Count = " << changeCount << endl;
	cout << "Creating Packet SZ = " << sizeof(FrameChangePacket) + (changeCount * sizeof(FrameChangeDesc)) - sizeof(FrameChangeDesc) << " / " << a.size() << endl;

	for (size_t i = 0, j = 0; i != a.size(); i++) {
		short db = (short)a[i] - (short)b[i];
		if (ABS16(db) > 20) {
			changePacket->changes[j++] = { i, b[i] };
		}
	}

	return std::shared_ptr<FrameChangePacket>(changePacket);
}

cv::Mat& operator>>(cv::Mat& frame, std::vector<uchar>& data) {
	data.resize(frame.total() * frame.elemSize());
	for (size_t i = 0; i != data.size(); i++) { data[i] = frame.ptr()[i]; }
	return frame;
}

cv::VideoCapture& operator>>(cv::VideoCapture& cap, std::vector<uchar>& data) {
	if (!cap.isOpened()) {
		throw std::runtime_error("Video Capture Device Is Not Open");
	}
	cv::Mat frame;
	cap >> frame;
	frame >> data;
	return cap;
}

cv::Mat& operator<<(cv::Mat& frame, const std::vector<uchar>& data) {
	size_t maxIdx = MIN(frame.total() * frame.elemSize(), data.size());

	for (size_t i = 0; i != maxIdx; i++) {
		frame.ptr()[i] = data[i];
	}

	return frame;
}

int main() {


	return -1;

	namedWindow("A", CV_WINDOW_AUTOSIZE);
	VideoCapture cap(0);
	Mat frame;

	vector<uchar> preData;
	vector<uchar> postData;

	uchar i = 0;
	while (true) {
		if ((i % (uchar)10) == 0) {
			cap >> frame;		// put cam data into buffer ; capture -> frame (matrix) -> data buffer
			frame >> preData;
			imshow("A", frame);
		}
		else {
			cap >> frame;
			frame >> postData;

			std::shared_ptr<FrameChangePacket> pkt = buildChangePacket(preData, postData);

			if (pkt.get() != NULL) {
				for (size_t i = 0; i != pkt.get()->changeCount; i++) {
					postData[pkt.get()->changes[i].idx] = pkt.get()->changes[i].val;
				}

				frame << postData;
				printf("ON!\n");
			}
			else {
				printf("Null Frame Change Packet\n");
			}

			imshow("A", frame);
		}

		waitKey(50);

		i++;
	}

}