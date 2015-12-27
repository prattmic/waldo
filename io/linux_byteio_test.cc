#include <stdlib.h>
#include <iostream>
#include <string>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "util/status_test.h"

using io::LinuxByteIO;

TEST(LinuxByteIOTest, ReadWriteFile) {
    char *tmpdir = getenv("TEST_TMPDIR");
    std::string filename;
    if (tmpdir) {
        filename = tmpdir;
    } else {
        filename = "/tmp";
    }
    filename += "/XXXXXX";

    char mut_filename[filename.size() + 1];
    filename.copy(mut_filename, filename.size());
    mut_filename[filename.size()] = '\0';

    std::cerr << "base template: " << mut_filename << "\n";

    int fd = mkstemp(mut_filename);
    ASSERT_GE(fd, 0) << "errno: " << errno;

    auto stator1 = LinuxByteIO::OpenFile(mut_filename);
    ASSERT_OK(stator1.status());

    LinuxByteIO writer = stator1.ConsumeValue();

    auto w = writer.Write('a');
    EXPECT_OK(w);
    w = writer.Write('b');
    EXPECT_OK(w);
    w = writer.Write('c');
    EXPECT_OK(w);

    auto stator2 = LinuxByteIO::OpenFile(mut_filename);
    ASSERT_OK(stator2.status());

    LinuxByteIO reader = stator2.ConsumeValue();

    auto r = reader.Read();
    EXPECT_OK(r.status());
    EXPECT_EQ('a', r.Value());

    r = reader.Read();
    EXPECT_OK(r.status());
    EXPECT_EQ('b', r.Value());

    r = reader.Read();
    EXPECT_OK(r.status());
    EXPECT_EQ('c', r.Value());

    r = reader.Read();
    EXPECT_NOT_OK(r.status());
    EXPECT_EQ(util::error::Code::RESOURCE_EXHAUSTED, r.status().error_code());
}
