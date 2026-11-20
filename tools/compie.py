"""
Công cụ gộp nhiều file .txt thành 1 file duy nhất.
Tự động:
  - Loại bỏ dòng trống, khoảng trắng thừa
  - Lọc trùng lặp (deduplicate) bảo đảm dung lượng tối ưu
  - Hỗ trợ gộp tất cả file trong một thư mục hoặc danh sách file chỉ định

Cách dùng:
  1. Chạy mặc định (gộp toàn bộ file txt trong thư mục 'library' thành 'library/merged.txt'):
     python tools/compie.py

  2. Gộp thư mục tùy chọn:
     python tools/compie.py --dir <duong_dan_thu_muc> --out <file_dich>
     Ví dụ:
     python tools/compie.py --dir library --out library/all_domains.txt

  3. Gộp các file cụ thể:
     python tools/compie.py file1.txt file2.txt file3.txt -o merged.txt
"""

import os
import sys
import argparse

def merge_txt_files(input_files: list, output_file: str, deduplicate: bool = True) -> None:
    if not input_files:
        print("[!] Không có file nào để gộp.")
        return

    print(f"[*] Bắt đầu gộp {len(input_files)} file:")
    for f in input_files:
        size_kb = os.path.getsize(f) / 1024 if os.path.exists(f) else 0
        print(f"    - {os.path.basename(f)} ({size_kb:.1f} KB)")
    print(f"[*] File xuất: {output_file}")

    seen = set() if deduplicate else None
    total_lines = 0
    written_lines = 0

    # Đảm bảo thư mục cha của output tồn tại
    out_dir = os.path.dirname(os.path.abspath(output_file))
    if out_dir and not os.path.exists(out_dir):
        os.makedirs(out_dir, exist_ok=True)

    with open(output_file, "w", encoding="utf-8", newline="\n") as out_f:
        for file_path in input_files:
            if not os.path.exists(file_path):
                print(f"[!] Bỏ qua (không tồn tại): {file_path}")
                continue

            with open(file_path, "r", encoding="utf-8", errors="ignore") as in_f:
                for line in in_f:
                    total_lines += 1
                    cleaned = line.strip()

                    # Bỏ qua dòng trống hoặc dòng chú thích (# hoặc !)
                    if not cleaned or cleaned.startswith("#") or cleaned.startswith("!"):
                        continue

                    if deduplicate:
                        cleaned_lower = cleaned.lower()
                        if cleaned_lower in seen:
                            continue
                        seen.add(cleaned_lower)

                    out_f.write(cleaned + "\n")
                    written_lines += 1

    out_size_mb = os.path.getsize(output_file) / (1024 * 1024)
    print(f"[+] Hoàn thành!")
    print(f"    - Tổng dòng đã đọc: {total_lines:,}")
    print(f"    - Dòng ghi vào file: {written_lines:,} (đã lọc trùng & dòng trống)")
    print(f"    - Kích thước file đích: {out_size_mb:.2f} MB")

def main():
    parser = argparse.ArgumentParser(description="Gộp nhiều file .txt thành 1 file duy nhất.")
    parser.add_argument("files", nargs="*", help="Danh sách file .txt cần gộp")
    parser.add_argument("--dir", "-d", help="Thư mục chứa các file .txt cần gộp")
    parser.add_argument("--out", "-o", help="Đường dẫn file .txt đầu ra")
    parser.add_argument("--no-dedup", action="store_true", help="Không lọc trùng lặp")

    args = parser.parse_args()

    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    target_files = []

    if args.dir:
        dir_path = args.dir if os.path.isabs(args.dir) else os.path.join(project_root, args.dir)
        if os.path.exists(dir_path):
            target_files = sorted([
                os.path.join(dir_path, f)
                for f in os.listdir(dir_path)
                if f.endswith(".txt") and not f.startswith("merged")
            ])
        else:
            print(f"[!] Không tìm thấy thư mục: {dir_path}")
            return
    elif args.files:
        target_files = args.files
    else:
        # Chế độ mặc định: quét toàn bộ file trong thư mục library/
        lib_dir = os.path.join(project_root, "library")
        if os.path.exists(lib_dir):
            target_files = sorted([
                os.path.join(lib_dir, f)
                for f in os.listdir(lib_dir)
                if f.endswith(".txt") and not f.startswith("merged")
            ])
        out_file = os.path.join(lib_dir, "merged.txt")

    if not args.out:
        if args.dir:
            out_file = os.path.join(args.dir, "merged.txt")
        elif not args.files:
            out_file = os.path.join(project_root, "library", "merged.txt")
        else:
            out_file = "merged.txt"
    else:
        out_file = args.out

    merge_txt_files(target_files, out_file, deduplicate=not args.no_dedup)

if __name__ == "__main__":
    main()
