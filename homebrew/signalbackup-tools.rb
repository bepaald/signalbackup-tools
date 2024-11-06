# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
class SignalbackupTools < Formula
  desc "A tool to work with Signal backup files"
  homepage "https://github.com/bepaald/signalbackup-tools"
  license "GPL-3.0-or-later"
  head "https://github.com/bepaald/signalbackup-tools.git", branch: "master"

  depends_on "cmake" =>:build
  depends_on "openssl@3"
  depends_on "sqlite"

  def install
    system "cmake", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    bin.install "build/signalbackup-tools"
  end

  test do
    # not a 'good' test, but not sure what else is possible here
    `#{bin}/signalbackup-tools --help`
    result=$?.success?
    assert *result
  end
end
