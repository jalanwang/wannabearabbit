#include <iostream>
#include <string>
#include <curl/curl.h>

/**
 * CURL 응답 데이터를 문자열로 저장하기 위한 콜백 함수
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

/**
 * 문자열을 URL 인코딩하는 헬퍼 함수
 */
std::string urlEncode(CURL* curl, const std::string& value) {
    char* output = curl_easy_escape(curl, value.c_str(), value.length());
    std::string res(output);
    curl_free(output);
    return res;
}

/**
 * 주차 입차 이벤트를 서버에 전송하는 함수
 * @param session_id 세션 ID
 * @param slot_id 주차 면 ID
 * @param start_time 입차 시간 (YYYY-MM-DD HH:MM:SS)
 * @param plate 번호판 숫자
 * @return 성공 여부
 */
bool sendParkEvent(int session_id, int slot_id, const std::string& start_time, const std::string& plate) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    bool success = false;

    curl = curl_easy_init();
    if (curl) {
        std::string base_url = "http://16.184.56.119:5000/api/assignment-in";
        
        // 쿼리 파라미터 구성
        std::string params = "?session_id=" + std::to_string(session_id);
        params += "&slot_id=" + std::to_string(slot_id);
        params += "&park_start_time=" + urlEncode(curl, start_time);
        params += "&license_plate=" + urlEncode(curl, plate);

        std::string full_url = base_url + params;

        // CURL 옵션 설정
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);

        // 요청 실행
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            std::cout << "[이벤트 전송 성공] Response: " << readBuffer << std::endl;
            success = true;
        } else {
            std::cerr << "[이벤트 전송 실패] 에러: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return success;
}

int main() {
    std::cout << "이벤트 대기 중..." << std::endl;

    // 예시: 특정 조건(QR 인식 등)이 만족되었다고 가정할 때 호출
    bool event_triggered = true; 

    if (event_triggered) {
        std::cout << "이벤트 발생! 서버에 데이터를 전송합니다." << std::endl;
        
        // 함수 호출을 통해 리퀘스트 전송
        sendParkEvent(5, 7, "2026-01-27 16:40:00", "999999");
    }

    return 0;
}
