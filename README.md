# MYLS – Mô phỏng lệnh ls(1) trên NetBSD

**Sinh viên:** Nguyễn Phúc An  
**Mã sinh viên:** 24IT002  
**Ngôn ngữ lập trình:** C  
**Hệ điều hành:** NetBSD  
**GitHub:** https://github.com/Npa2912/NguyenPhucAn_24IT002_midterm

## 1. Giới thiệu

Dự án xây dựng chương trình `myls` bằng ngôn ngữ C, mô phỏng lệnh `ls(1)` trên hệ điều hành NetBSD.

Chương trình hỗ trợ liệt kê tệp và thư mục, hiển thị thông tin chi tiết, sắp xếp, xử lý liên kết tượng trưng và duyệt thư mục đệ quy.

## 2. Cấu trúc dự án

```text
NguyenPhucAn_24IT002_midterm/
├── src/           # Mã nguồn chương trình C
├── include/       # Các file header
├── tests/         # Dữ liệu và script kiểm thử
├── Makefile       # Biên dịch chương trình
├── README.md      # Hướng dẫn sử dụng
└── .gitignore
```

## 3. Yêu cầu môi trường

Chương trình được xây dựng để chạy trên hệ điều hành NetBSD.

Các công cụ cần có:

- Trình biên dịch C (`cc` hoặc `gcc`)
- `make`
- `git` để tải mã nguồn từ GitHub
- `sh` để chạy kiểm thử

Kiểm tra môi trường:

```sh
uname -r
command -v cc
command -v make
command -v git
```

Nếu chưa có Git, có thể tải dự án dưới dạng ZIP từ GitHub và giải nén vào NetBSD.

## 4. Tải mã nguồn và biên dịch

**Bước 1: Tải dự án từ GitHub**

Mở Terminal trên NetBSD và chạy:

```sh
cd ~
git clone https://github.com/Npa2912/NguyenPhucAn_24IT002_midterm.git
```

**Bước 2: Truy cập thư mục dự án**

```sh
cd NguyenPhucAn_24IT002_midterm
```

**Bước 3: Biên dịch chương trình**

```sh
make
```

Sau khi biên dịch thành công, file thực thi `myls` sẽ được tạo trong thư mục dự án.

Kiểm tra bằng lệnh:

```sh
ls -l myls
```

## 5. Hướng dẫn sử dụng

**Cú pháp:**

```sh
./myls [options] [file ...]
```

**Một số ví dụ:**

```sh
./myls
./myls -a
./myls -l
./myls -la
./myls -lh
./myls -lt src
./myls -ltr src
./myls -R tests
./myls -Rla tests
./myls -F src
./myls -i src
./myls -s src
./myls -n src
```

Chương trình triển khai các tùy chọn theo tài liệu NetBSD `ls(1)`:

`-A -a -c -d -F -f -h -i -k -l -n -q -R -r -S -s -t -u -w`

Các tùy chọn có thể kết hợp với nhau, ví dụ `-la`, `-ltr`, `-Rla`.

## 6. Kiểm thử chương trình

Chương trình MYLS hiện được kiểm thử thủ công trên hệ điều hành NetBSD bằng cách thực thi các tùy chọn và đối chiếu kết quả với lệnh `ls` gốc.

**Kiểm thử chức năng liệt kê chi tiết:**

```sh
./myls -la tests
ls -la tests
```

Kết quả thực nghiệm cho thấy hai lệnh hiển thị tương đương về danh sách tệp, thư mục, quyền truy cập, chủ sở hữu, kích thước và thời gian sửa đổi trong trường hợp kiểm thử này.

**Kiểm thử các chức năng khác:**

```sh
./myls
./myls -la
./myls -lh /etc
./myls -R tests
./myls -S /etc
./myls -F
```

**Tình trạng kiểm thử tự động:**

Dự án có tệp `tests/test.sh` và quy tắc `make test` trong Makefile. Tuy nhiên, tệp `tests/test.sh` hiện chưa có nội dung nên chưa triển khai bộ kiểm thử tự động.

Trong các phiên bản tiếp theo, dự án sẽ bổ sung các trường hợp kiểm thử tự động nhằm đánh giá đầy đủ các tùy chọn và khả năng xử lý lỗi của chương trình.
## 7. Dọn dẹp và biên dịch lại

Xóa các file đã biên dịch:

```sh
make clean
```

Biên dịch lại chương trình:

```sh
make
```

Lệnh `make clean` không xóa mã nguồn.

## 8. Lưu ý

- Chạy các lệnh trong thư mục gốc của dự án, nơi chứa file `Makefile`.
- Chương trình không yêu cầu thư viện bên thứ ba.
- Cần kiểm thử trực tiếp trên NetBSD để xác nhận khả năng tương thích.

---

**Sinh viên thực hiện:** Nguyễn Phúc An – 24IT002
