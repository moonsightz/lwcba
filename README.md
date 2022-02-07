# lwcba (Launch With ClipBoard data as Arguments)

Windows application to launch application with clipboard data as arguments.

## What's this

Example:

```
lwcba.exe "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" -inPrivate
```

When URL text is copied to clipboard, this command launches Edge in private mode with the URL.  If 'https://www.icann.org/' is on clipboard, ICANN web page is opened with Edge in private mode.


If you want to use application with copied text or files, this may reduce some steps.


If you copy files in explorer, their paths are passed as arguments.


By Making shortcut and specifying application path and args in target, you can set copied text or file paths by 'copy text => launch the shortcut'.


## Install

Copy lwcba.exe to a directory where you want to use.

.exe is not signed(code signing), so the execution may be blocked.

If you get the error that there is no Visual C Runtime, install it(https://aka.ms/vs/17/release/vc_redist.x64.exe or https://visualstudio.microsoft.com/downloads/)



## Restriction
The application path to launch must be within 260 characters.
(In programming language C, executed file path is set as `argv[0]`.  But, it seems like that `CreateProcessA()` does not pass it as `argv[0]` when `lpApplicationName` is set.  Ref. https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa)


With too long text or file path, it may fail (Buffer size is 32767).
