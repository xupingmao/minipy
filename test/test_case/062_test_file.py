
def test_file_read():
    fpath = "./test/test_case/062_input.txt"
    fp = open(fpath)
    text = fp.read(5)
    print("text:", text)
    assert text == "hello"

    rest = fp.read()
    print("rest:", rest)
    assert rest == ",world"

    fp.close()
    print("SUCCESS")

test_file_read()
