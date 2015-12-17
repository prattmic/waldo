#include <stdlib.h>
#include <iostream>
#include <string>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"

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
    ASSERT_TRUE(stator1.ok());

    LinuxByteIO writer = stator1.ConsumeValue();

    auto w = writer.Write('a');
    EXPECT_TRUE(w.ok()) << w.ToString();
    w = writer.Write('b');
    EXPECT_TRUE(w.ok()) << w.ToString();
    w = writer.Write('c');
    EXPECT_TRUE(w.ok()) << w.ToString();

    auto stator2 = LinuxByteIO::OpenFile(mut_filename);
    ASSERT_TRUE(stator2.ok());

    LinuxByteIO reader = stator2.ConsumeValue();

    auto r = reader.Read();
    EXPECT_TRUE(r.ok());
    EXPECT_EQ('a', r.Value()) << r.status().ToString();

    r = reader.Read();
    EXPECT_TRUE(r.ok());
    EXPECT_EQ('b', r.Value()) << r.status().ToString();

    r = reader.Read();
    EXPECT_TRUE(r.ok());
    EXPECT_EQ('c', r.Value()) << r.status().ToString();

    r = reader.Read();
    EXPECT_FALSE(r.ok());
    EXPECT_EQ(util::error::Code::RESOURCE_EXHAUSTED, r.status().error_code());
}
