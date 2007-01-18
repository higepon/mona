#!/usr/bin/ruby
require 'pp'
MONA_DIR = "mona"
CONTRIB_DIR = "contrib"
M_REPOS = "https://monaos.svn.sourceforge.net/svnroot/monaos/branches/mona-stdio"
C_REPOS = "https://monaos.svn.sourceforge.net/svnroot/monaos/branches/contrib-stdio"

def qemu(revision)
  system("/usr/bin/qemu -cdrom ~/tmp/#{revision}/mona/tool/mkimg/mona.iso -fda ~/tmp/#{revision}/mona/tool/mkimg/mona.img -boot d -k ja ")
end

def fetch_and_execute(revision)
  dir = Dir.pwd
  p "===================# revision #{revision} ==================="
  dir_exist = system("mkdir #{revision}") == 1
  Dir.chdir("#{revision}")
  unless (dir_exist)
    system("svn co -r#{revision} #{M_REPOS} #{MONA_DIR}")
    system("svn co -r#{revision} #{C_REPOS} #{CONTRIB_DIR}")
  end
  system("cd mona && ./configure --mingw-prefix=/usr/bin/i586-mingw32msvc- && make");
  system("cd contrib && make");
  qemu(revision)
  Dir.chdir(dir)
end


r = ARGV.shift

if (r && r == 'qemu')
  qemu(ARGV.shift)
elsif (r)
  fetch_and_execute(r)
else
  revisions = []

  `svn log -q #{M_REPOS}`.each_line {|i|
    if m = i.match(/r(\d+)/)
      revisions << m[1]
    end
  }
  revisions.each {|i|
      fetch_and_execute(i)
  }
end

#  system("rm -rf #{MONA_DIR}")
#  system("rm -rf #{CONTRIB_DIR}")
