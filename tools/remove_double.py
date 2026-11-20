"""
Công cụ lọc trùng, loại bỏ dòng chứa ký tự không hợp lệ (*, ^, $) 
và sắp xếp danh sách domain/dòng trong file theo thứ tự từ A -> Z.
Mặc định áp dụng cho file: library/merged.txt

Cách dùng:
  1. Chạy mặc định:
     python tools/remove_double.py

  2. Chạy với file tùy chọn:
     python tools/remove_double.py <duong_dan_file>
     Ví dụ:
     python tools/remove_double.py library/merged.txt
"""

import os
import sys
import tempfile
import shutil

def deduplicate_and_sort(file_path: str) -> None:
    if not os.path.exists(file_path):
        print(f"[!] Không tìm thấy file: {file_path}")
        return

    print(f"[*] Đang đọc file: {file_path}")

    unique_lines = set()
    total_lines = 0
    removed_invalid_count = 0

    # Ký tự không hợp lệ cần loại bỏ
    invalid_chars = {'*', '^', '$'}

    # Đọc, lọc dòng chứa *, ^, $ và lọc trùng
    with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            total_lines += 1
            cleaned = line.strip()
            if not cleaned:
                continue

            # Bỏ dòng chứa bất kỳ ký tự nào trong *, ^, $
            if any(ch in cleaned for ch in invalid_chars):
                removed_invalid_count += 1
                continue

            # Đưa về chữ thường để tránh trùng hoa/thường và chuẩn hóa domain
            unique_lines.add(cleaned.lower())

    duplicate_count = total_lines - removed_invalid_count - len(unique_lines)
    print(f"[*] Tổng số dòng: {total_lines:,}")
    print(f"[*] Số dòng bị loại bỏ (chứa *, ^, $): {removed_invalid_count:,}")
    print(f"[*] Số dòng trùng lặp đã loại bỏ: {duplicate_count:,}")
    print(f"[*] Số dòng hợp lệ duy nhất: {len(unique_lines):,}")

    # Sắp xếp theo thứ tự alphabet từ a -> z
    print("[*] Đang sắp xếp từ A -> Z...")
    sorted_lines = sorted(unique_lines)

    # Ghi ra file tạm rồi ghi đè an toàn
    file_dir = os.path.dirname(os.path.abspath(file_path))
    temp_fd, temp_path = tempfile.mkstemp(dir=file_dir, prefix="sort_", suffix=".tmp")

    try:
        with open(temp_fd, "w", encoding="utf-8", newline="\n") as out_f:
            for item in sorted_lines:
                out_f.write(item + "\n")

        # Ghi đè lên file gốc
        shutil.move(temp_path, file_path)
        new_size_mb = os.path.getsize(file_path) / (1024 * 1024)
        print(f"[+] Thành công! File đã được cập nhật ({new_size_mb:.2f} MB).")
    except Exception as e:
        if os.path.exists(temp_path):
            os.remove(temp_path)
        print(f"[!] Lỗi khi ghi file: {e}")

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    default_target = os.path.join(project_root, "library", "merged.txt")

    target_file = sys.argv[1] if len(sys.argv) > 1 else default_target
    deduplicate_and_sort(target_file)
