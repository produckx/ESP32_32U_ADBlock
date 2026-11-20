"""
Công cụ tìm và thay thế chuỗi/ký tự trong file (hỗ trợ file dung lượng lớn).
Cách dùng:
  1. Chạy trực tiếp (mặc định xử lý list2.txt bỏ '0.0.0.0 '):
     python tools/repalce_tool.py

  2. Chạy với tham số tùy chọn:
     python tools/repalce_tool.py <file_path> <old_str> <new_str>
     Ví dụ:
     python tools/repalce_tool.py library/list2.txt "0.0.0.0 " ""
"""

import os
import sys
import tempfile
import shutil

def replace_in_file(file_path: str, old_str: str, new_str: str) -> None:
    if not os.path.exists(file_path):
        print(f"[!] Không tìm thấy file: {file_path}")
        return

    print(f"[*] Đang xử lý file: {file_path}")
    print(f"[*] Thay thế: {repr(old_str)} -> {repr(new_str)}")

    # Tạo file tạm trong cùng thư mục để đảm bảo di chuyển an toàn trên Windows
    file_dir = os.path.dirname(os.path.abspath(file_path))
    temp_fd, temp_path = tempfile.mkstemp(dir=file_dir, prefix="replace_", suffix=".tmp")

    total_lines = 0
    match_count = 0

    try:
        with open(temp_fd, "w", encoding="utf-8", newline="\n") as out_f:
            with open(file_path, "r", encoding="utf-8", errors="ignore") as in_f:
                for line in in_f:
                    total_lines += 1
                    if old_str in line:
                        match_count += line.count(old_str)
                        line = line.replace(old_str, new_str)
                    out_f.write(line)

        # Ghi đè file tạm lên file gốc
        shutil.move(temp_path, file_path)
        print(f"[+] Hoàn thành! Đã quét {total_lines:,} dòng, thay thế {match_count:,} lần xuất hiện.")
    except Exception as e:
        if os.path.exists(temp_path):
            os.remove(temp_path)
        print(f"[!] Lỗi trong quá trình thay thế: {e}")

if __name__ == "__main__":
    # Đường dẫn mặc định trỏ tới library/list2.txt nếu không truyền tham số
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    default_file = os.path.join(project_root, "library", "list4.txt")
    default_old = "^"
    default_new = ""

    if len(sys.argv) >= 4:
        target_file = sys.argv[1]
        target_old = sys.argv[2]
        target_new = sys.argv[3]
    elif len(sys.argv) == 2 and sys.argv[1] in ("-h", "--help"):
        print(__doc__)
        sys.exit(0)
    else:
        # Mặc định
        target_file = default_file
        target_old = default_old
        target_new = default_new

    replace_in_file(target_file, target_old, target_new)
