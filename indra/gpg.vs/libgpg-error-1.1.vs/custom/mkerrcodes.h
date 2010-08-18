/* Output of mkerrcodes.awk.  DO NOT EDIT.  */

static struct
  {
    int err;
    const char *err_sym;
  } err_table[] = 
{
  { 7, "GPG_ERR_E2BIG" },
  { 13, "GPG_ERR_EACCES" },
  { 98, "GPG_ERR_EADDRINUSE" },
  { 99, "GPG_ERR_EADDRNOTAVAIL" },
  { 68, "GPG_ERR_EADV" },
  { 97, "GPG_ERR_EAFNOSUPPORT" },
  { 11, "GPG_ERR_EAGAIN" },
  { 114, "GPG_ERR_EALREADY" },
  { 52, "GPG_ERR_EBADE" },
  { 9, "GPG_ERR_EBADF" },
  { 77, "GPG_ERR_EBADFD" },
  { 74, "GPG_ERR_EBADMSG" },
  { 53, "GPG_ERR_EBADR" },
  { 56, "GPG_ERR_EBADRQC" },
  { 57, "GPG_ERR_EBADSLT" },
  { 59, "GPG_ERR_EBFONT" },
  { 16, "GPG_ERR_EBUSY" },
  { 125, "GPG_ERR_ECANCELED" },
  { 10, "GPG_ERR_ECHILD" },
  { 44, "GPG_ERR_ECHRNG" },
  { 70, "GPG_ERR_ECOMM" },
  { 103, "GPG_ERR_ECONNABORTED" },
  { 111, "GPG_ERR_ECONNREFUSED" },
  { 104, "GPG_ERR_ECONNRESET" },
  { 35, "GPG_ERR_EDEADLK" },
  { 35, "GPG_ERR_EDEADLOCK" },
  { 89, "GPG_ERR_EDESTADDRREQ" },
  { 33, "GPG_ERR_EDOM" },
  { 73, "GPG_ERR_EDOTDOT" },
  { 122, "GPG_ERR_EDQUOT" },
  { 17, "GPG_ERR_EEXIST" },
  { 14, "GPG_ERR_EFAULT" },
  { 27, "GPG_ERR_EFBIG" },
  { 112, "GPG_ERR_EHOSTDOWN" },
  { 113, "GPG_ERR_EHOSTUNREACH" },
  { 43, "GPG_ERR_EIDRM" },
  { 84, "GPG_ERR_EILSEQ" },
  { 115, "GPG_ERR_EINPROGRESS" },
  { 4, "GPG_ERR_EINTR" },
  { 22, "GPG_ERR_EINVAL" },
  { 5, "GPG_ERR_EIO" },
  { 106, "GPG_ERR_EISCONN" },
  { 21, "GPG_ERR_EISDIR" },
  { 120, "GPG_ERR_EISNAM" },
  { 51, "GPG_ERR_EL2HLT" },
  { 45, "GPG_ERR_EL2NSYNC" },
  { 46, "GPG_ERR_EL3HLT" },
  { 47, "GPG_ERR_EL3RST" },
  { 79, "GPG_ERR_ELIBACC" },
  { 80, "GPG_ERR_ELIBBAD" },
  { 83, "GPG_ERR_ELIBEXEC" },
  { 82, "GPG_ERR_ELIBMAX" },
  { 81, "GPG_ERR_ELIBSCN" },
  { 48, "GPG_ERR_ELNRNG" },
  { 40, "GPG_ERR_ELOOP" },
  { 124, "GPG_ERR_EMEDIUMTYPE" },
  { 24, "GPG_ERR_EMFILE" },
  { 31, "GPG_ERR_EMLINK" },
  { 90, "GPG_ERR_EMSGSIZE" },
  { 72, "GPG_ERR_EMULTIHOP" },
  { 36, "GPG_ERR_ENAMETOOLONG" },
  { 119, "GPG_ERR_ENAVAIL" },
  { 100, "GPG_ERR_ENETDOWN" },
  { 102, "GPG_ERR_ENETRESET" },
  { 101, "GPG_ERR_ENETUNREACH" },
  { 23, "GPG_ERR_ENFILE" },
  { 55, "GPG_ERR_ENOANO" },
  { 105, "GPG_ERR_ENOBUFS" },
  { 50, "GPG_ERR_ENOCSI" },
  { 61, "GPG_ERR_ENODATA" },
  { 19, "GPG_ERR_ENODEV" },
  { 2, "GPG_ERR_ENOENT" },
  { 8, "GPG_ERR_ENOEXEC" },
  { 37, "GPG_ERR_ENOLCK" },
  { 67, "GPG_ERR_ENOLINK" },
  { 123, "GPG_ERR_ENOMEDIUM" },
  { 12, "GPG_ERR_ENOMEM" },
  { 42, "GPG_ERR_ENOMSG" },
  { 64, "GPG_ERR_ENONET" },
  { 65, "GPG_ERR_ENOPKG" },
  { 92, "GPG_ERR_ENOPROTOOPT" },
  { 28, "GPG_ERR_ENOSPC" },
  { 63, "GPG_ERR_ENOSR" },
  { 60, "GPG_ERR_ENOSTR" },
  { 38, "GPG_ERR_ENOSYS" },
  { 15, "GPG_ERR_ENOTBLK" },
  { 107, "GPG_ERR_ENOTCONN" },
  { 20, "GPG_ERR_ENOTDIR" },
  { 39, "GPG_ERR_ENOTEMPTY" },
  { 118, "GPG_ERR_ENOTNAM" },
  { 88, "GPG_ERR_ENOTSOCK" },
  { 95, "GPG_ERR_ENOTSUP" },
  { 25, "GPG_ERR_ENOTTY" },
  { 76, "GPG_ERR_ENOTUNIQ" },
  { 6, "GPG_ERR_ENXIO" },
  { 95, "GPG_ERR_EOPNOTSUPP" },
  { 75, "GPG_ERR_EOVERFLOW" },
  { 1, "GPG_ERR_EPERM" },
  { 96, "GPG_ERR_EPFNOSUPPORT" },
  { 32, "GPG_ERR_EPIPE" },
  { 71, "GPG_ERR_EPROTO" },
  { 93, "GPG_ERR_EPROTONOSUPPORT" },
  { 91, "GPG_ERR_EPROTOTYPE" },
  { 34, "GPG_ERR_ERANGE" },
  { 78, "GPG_ERR_EREMCHG" },
  { 66, "GPG_ERR_EREMOTE" },
  { 121, "GPG_ERR_EREMOTEIO" },
  { 85, "GPG_ERR_ERESTART" },
  { 30, "GPG_ERR_EROFS" },
  { 108, "GPG_ERR_ESHUTDOWN" },
  { 94, "GPG_ERR_ESOCKTNOSUPPORT" },
  { 29, "GPG_ERR_ESPIPE" },
  { 3, "GPG_ERR_ESRCH" },
  { 69, "GPG_ERR_ESRMNT" },
  { 116, "GPG_ERR_ESTALE" },
  { 86, "GPG_ERR_ESTRPIPE" },
  { 62, "GPG_ERR_ETIME" },
  { 110, "GPG_ERR_ETIMEDOUT" },
  { 109, "GPG_ERR_ETOOMANYREFS" },
  { 26, "GPG_ERR_ETXTBSY" },
  { 117, "GPG_ERR_EUCLEAN" },
  { 49, "GPG_ERR_EUNATCH" },
  { 87, "GPG_ERR_EUSERS" },
  { 11, "GPG_ERR_EWOULDBLOCK" },
  { 18, "GPG_ERR_EXDEV" },
  { 54, "GPG_ERR_EXFULL" },
};
