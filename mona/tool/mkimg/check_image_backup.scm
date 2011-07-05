(import (rnrs)
        (mosh)
        (mosh control)
        (mosh process)
        (mosh file))

(define (main args)
  (let1 latest-backup (string-append "/home/taro/Dropbox/mona_images/" (car (list-sort string>=? (directory-list "/home/taro/Dropbox/mona_images/"))))
    (when (> (file-stat-mtime latest-backup) (file-stat-ctime "./fat32_user.img"))
      (call-process (format "bash restore.sh ~a" latest-backup)))))

(when (file-exists? "/home/taro/Dropbox/mona_images/")
  (main (command-line)))
