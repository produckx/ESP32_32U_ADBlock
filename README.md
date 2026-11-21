# ProDuckX-DNS — Thiết Bị Chặn Quảng Cáo & Mã Độc DNS Trên ESP32

**ProDuckX-DNS** là giải pháp phần cứng DNS Sinkhole hiệu năng cao chạy trên vi điều khiển **ESP32** (Flash 4MB), hoạt động tương tự Pi-hole / AdGuard Home nhưng nhỏ gọn, tiết kiệm điện năng và khởi động tức thì.

---

## 🚀 Tính Năng Chính

- **DNS Sinkhole Siêu Nhanh**:
  - Tìm kiếm nhị phân trên tập hash 40-bit (5 byte FNV-1a) lưu trực tiếp trên bộ nhớ flash LittleFS.
  - Bộ nhớ phân vùng tối ưu chứa thoải mái hơn **500.000 tên miền** quảng cáo / mã độc.
  - Phản hồi cực nhanh, độ trễ thấp (< 2ms đối với query được cache/sinkhole).
- **Trang Quản Trị Web (Dashboard)**:
  - Truy cập qua `http://produckx.local` hoặc địa chỉ IP của ESP32.
  - Xác thực bảo mật bằng phiên đăng nhập (Session Cookie 2 giờ) và chống CSRF.
  - Giám sát thời gian thực: **% CPU** (đo qua FreeRTOS idle hook dual-core), **% RAM**, tổng lượt truy vấn bị chặn/cho phép, danh sách thiết bị truy cập (Clients Table).
  - Quản lý **Custom Blocklist** (Chặn tên miền tùy chọn) & **Whitelist** (Tên miền luôn cho phép, ưu tiên cao hơn blocklist).
  - Xem lịch sử gần đây bị chặn (**Recently Blocked**).
- **Cấu Hình Mạng Linh Hoạt**:
  - Hỗ trợ thiết lập **IP Tĩnh (Static IP)**: IP, Gateway, Subnet, DNS hoặc chuyển về DHCP ngay trên giao diện web.
  - Cơ chế **Captive Portal**: Tự động phát WiFi AP `ProDuckX-DNS-XXXX` để cài đặt mạng mới khi không có WiFi hoặc khi nhấn nút **Forget WiFi**.
- **Tùy Biến Trang Chuyển Hướng Chặn (Blocked Page Redirect)**:
  - Chế độ mặc định: Chặn im lặng (`0.0.0.0`).
  - Chế độ Redirect: Tự động chuyển hướng trình duyệt sang trang thông báo (ví dụ `https://block.yourdomain.com?domain=...`) khi truy cập web bị chặn.
- **Tự Động Cập Nhật Blocklist Từ Xa**:
  - Tải file `blocklist.bin` định kỳ qua HTTP/HTTPS (ví dụ GitHub Releases).

---

## 🛠️ Yêu Cầu Môi Trường

- **Phần cứng**: ESP32 Dev Module (Flash 4MB).
- **Phần mềm**:
  - Python 3.8+.
  - PlatformIO Core (CLI) hoặc extension PlatformIO trên VS Code.

### Kích hoạt môi trường ảo `venv` và cài PlatformIO

Nếu chưa có môi trường ảo, tạo mới tại thư mục gốc dự án:

```powershell
# Tạo venv (chỉ làm 1 lần)
python -m venv venv

# Kích hoạt venv (PowerShell)
.\venv\Scripts\Activate.ps1

# Nếu PowerShell chặn script, cho phép trong phiên hiện tại rồi kích hoạt lại
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\venv\Scripts\Activate.ps1

# Cài PlatformIO Core vào venv
pip install -U platformio

# Kiểm tra
pio --version
```

> Sau khi kích hoạt, có thể dùng `pio ...` trực tiếp. Nếu không kích hoạt, dùng đường dẫn đầy đủ `.\venv\Scripts\pio.exe ...` như trong các lệnh bên dưới.

---

## ⚙️ Cấu Hình Trước Khi Nạp

### 1. File cấu hình bảo mật `src/secrets.h`
Tạo hoặc chỉnh sửa file `src/secrets.h` từ file mẫu:

```cpp
#pragma once

// Cấu hình WiFi dự phòng (nếu chưa cấu hình qua Captive Portal)
#define WIFI_SSID "Ten_Wifi_Cua_Ban"
#define WIFI_PASS "Mat_Khau_Wifi"

// Tài khoản đăng nhập Dashboard (http://produckx.local)
#define WEB_USER  "admin"
#define WEB_PASS  "Mat_Khau_Quan_Tri_Moi"

// Mật khẩu cập nhật qua mạng (OTA)
#define OTA_PASS  "Mat_Khau_OTA"
```

---

## 📦 Quy Trình Biên Dịch & Nạp Code

Dự án sử dụng PlatformIO trong môi trường ảo `.\venv`. Mở PowerShell tại thư mục gốc dự án:

### Bước 1: Nạp Firmware (Chương trình chính)
Nạp mã nguồn firmware và cấu trúc phân vùng mới vào ESP32:
```powershell
.\venv\Scripts\pio.exe run -t upload
```

*(Tùy chọn: Mở màn hình Serial Monitor xem log khởi động)*:
```powershell
.\venv\Scripts\pio.exe run -t upload -t monitor
```

### Bước 2: Chuẩn Bị & Nạp Danh Sách Chặn (LittleFS)
File danh sách chặn `data/blocklist.bin` cần được nạp vào bộ nhớ flash LittleFS:

1. **Biên dịch danh sách tên miền thành file nhị phân**:
   ```powershell
   .\venv\Scripts\python.exe tools\build_blocklist.py
   ```
   *Lệnh này sẽ đọc các tên miền từ `library/merged.txt`, lọc bỏ cú pháp thừa, băm mã 5-byte và xuất ra `data/blocklist.bin`.*

2. **Nạp file hệ thống vào ESP32**:
   ```powershell
   .\venv\Scripts\pio.exe run -t uploadfs
   ```

> **Lưu ý**: Chỉ cần chạy lệnh `uploadfs` khi lần đầu nạp board mới hoặc khi bạn muốn thay đổi danh sách `blocklist.bin` qua cổng nạp dây USB.

---

## 🧰 Bộ Công Cụ Tiện Ích (`tools/`)

Trong thư mục `tools/` cung cấp các script Python xử lý danh sách domain dung lượng lớn:

1. **Gộp nhiều file danh sách**:
   ```powershell
   # Gộp toàn bộ file .txt trong library/ thành library/merged.txt (tự động lọc trùng)
   python tools/compie.py
   ```

2. **Lọc trùng, loại bỏ ký tự lạ và sắp xếp A-Z**:
   ```powershell
   # Loại bỏ các dòng chứa *, ^, $ và sắp xếp danh sách từ A -> Z
   python tools/remove_double.py
   ```

3. **Thay thế ký tự trong file lớn**:
   ```powershell
   # Thay thế chuỗi cũ thành chuỗi mới theo luồng stream
   python tools/repalce_tool.py library/list2.txt "0.0.0.0 " ""
   ```

4. **Biên dịch blocklist sang file nhị phân cho ESP32**:
   ```powershell
   # Xuất file nhị phân data/blocklist.bin (~2.4 MB cho 500k domain)
   python tools/build_blocklist.py
   ```

---

## 🖥️ Hướng Dẫn Sử Dụng & Thiết Lập

### 1. Kết Nối WiFi Lần Đầu (Captive Portal)
Nếu ESP32 chưa có cấu hình WiFi:
1. ESP32 sẽ phát một mạng WiFi mở có tên: `ProDuckX-DNS-XXXX`.
2. Dùng điện thoại hoặc máy tính kết nối vào mạng WiFi này.
3. Trang cấu hình WiFi sẽ tự động bật lên (hoặc truy cập `http://192.168.4.1`).
4. Chọn mạng WiFi nhà bạn, nhập mật khẩu và bấm **Connect**. Thiết bị sẽ tự khởi động lại và kết nối vào mạng.

### 2. Truy Cập Trang Quản Trị
- Mở trình duyệt và truy cập: **`http://produckx.local`** (hoặc địa chỉ IP được cấp cho ESP32).
- Nhập `WEB_USER` và `WEB_PASS` đã đặt trong `src/secrets.h`.

### 3. Cấu Hình Thiết Bị Làm DNS Server
Để toàn bộ mạng gia đình được bảo vệ:
- Đăng nhập vào Router / Modem mạng của bạn.
- Tìm mục cấu hình **DHCP Server** -> Đặt **Primary DNS (DNS chính)** thành địa chỉ IP của ESP32.
- (Khuyên dùng) Đặt IP tĩnh cho ESP32 trong router hoặc đặt tại mục **NETWORK — STATIC IP** trên trang quản trị.

### 4. Tùy Biến Trang Thông Báo Chặn (Blocked Page)
- File `index.html` đi kèm là trang giao diện thông báo chặn chuyên nghiệp với thương hiệu ProDuckX, hiệu ứng animation và tự động nhận diện tên miền bị chặn qua query string.
- Bạn có thể tải file `index.html` và thư mục `images/` lên Hosting / Cloudflare Pages / VPS (ví dụ: `https://block.yourdomain.com`).
- Vào Dashboard ESP32 -> Tại mục **BLOCKED PAGE REDIRECT**, nhập URL `https://block.yourdomain.com` và bấm **Save**.
- Khi một thiết bị truy cập vào trang quảng cáo / web cấm qua giao thức HTTP, ESP32 sẽ tự động chuyển hướng sang:
  `https://block.yourdomain.com?domain=ads.example.com`

---

## ❓ Câu Hỏi Thường Gặp (Troubleshooting)

**Q: Khi xóa tên miền khỏi Custom Blocklist nhưng máy tính vẫn báo bị chặn?**
> **A**: Do máy tính / điện thoại đã lưu cache DNS của IP `0.0.0.0` trước đó. Bạn có thể mở CMD trên Windows chạy `ipconfig /flushdns`, hoặc bật/tắt chế độ máy bay trên điện thoại để xóa cache DNS.

**Q: Muốn đổi mạng WiFi khác cho ESP32 thì làm thế nào?**
> **A**: Vào Dashboard `http://produckx.local`, nhấn nút **Forget WiFi** ở cuối trang. ESP32 sẽ tự khởi động lại vào chế độ phát WiFi `ProDuckX-DNS-XXXX` để bạn cấu hình mạng mới mà không cần cắm cáp nạp lại code.

**Q: Lỗi `File system is full` khi chạy `uploadfs`?**
> **A**: Dự án đã tối ưu bảng phân vùng `partitions.csv` (2.68 MB cho LittleFS). Đảm bảo bạn đã nạp firmware mới (`pio run -t upload`) trước khi chạy `uploadfs`.
