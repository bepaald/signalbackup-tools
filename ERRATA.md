## ERRATA

While this tool has been used to fix a number of bugs in Signal Android, it's more experimental features have also introduced issues in databases at times. When this happens, most often the backup will fail to reestore properly. Unfortunately, sometimes a flaw is introduced into the database that only causes a problem at some later point in time. If you believe you are running in to any of these and need help, of course do not hesitate to open an issue.

- Main symptom: cloud backup fails to be created for a database that was previouly merged (with `--importfromthreads`) with this tool.

During backup merging, the column `ringer` in the `call` table was not treated as referencing a `recipient._id`. This could cause an entry in the `call` table to exist with a `ringer` that reference a non-existing (or invalid) recipient.

  See [#332](https://github.com/bepaald/signalbackup-tools/issues/332#issuecomment-3426285375) for details and a fix.
  
- Main symptom: Signal crashes randomly for a database which was created by `--importfromdesktop` _and_ `--importdesktopcontacts`. The debug log wil show a `java.lang.AssertionError` somewhere in `StorageForcePushJob` or `StorageSyncJob`.

  While importing group-recipients from Signal Desktop, sometimes they have no `storageID` set in the Signal Desktop database, while this is a required column (`storage_service_id`) for the Android database. The tool erroneously imported these contacts anyway, setting the `storage_service_id` to `NULL`.

  A fix is to identify the offending recipient, and delete it (along with the thread for that group). See [#341](https://github.com/bepaald/signalbackup-tools/issues/341).

  Alternatively, _but untested_: while a `storageID` was not present when the contact was imported from Signal Desktop originally, it may be present now. In that case extracting that and updating the Android database should probably also work. Also, it may actually work to set the `storage_service_id` to some new random 16 byte key.
