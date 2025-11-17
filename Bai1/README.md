# Bài 1 - Chương trình xây dựng chỉ mục từ

## Mô tả
Chương trình tạo chỉ mục từ (index) từ văn bản đầu vào, loại bỏ stopwords và xuất kết quả theo thứ tự alphabet.

## Cách biên dịch

### Sử dụng Makefile (khuyến nghị):
```bash
make
```

### Biên dịch thủ công:
```bash
gcc -Wall -Wextra -std=c11 -g -o Bai1 Bai1.c
```

## Cách chạy

### Cú pháp:
```bash
./Bai1 <file_van_ban> <file_stopwords>
```

### Ví dụ lệnh:
```bash
# Ví dụ 1: Sử dụng file văn bản mẫu
./Bai1 vanban.txt stopw.txt

# Ví dụ 2: Sử dụng file alice30.txt
./Bai1 alice30.txt stopw.txt

# Ví dụ 3: Sử dụng file thuchanh1.txt
./Bai1 thuchanh1.txt stopw.txt
```

### Xem kết quả:
```bash
cat result.txt
```

## Dọn dẹp

### Xóa file biên dịch:
```bash
make clean
```

### Xóa thủ công:
```bash
rm -f Bai1 Bai1.o
```

## Định dạng file

- **File văn bản**: Text thuần, mỗi dòng chứa nội dung cần đánh chỉ mục
- **File stopwords**: Mỗi dòng một stopword cần loại bỏ
- **File kết quả**: Danh sách từ theo alphabet với số lần xuất hiện và số dòng

## Lưu ý
- Chương trình tự động chuyển tất cả từ về chữ thường
- Loại bỏ các ký tự đặc biệt, chỉ giữ lại chữ cái và số
- Stopwords sẽ không xuất hiện trong kết quả
