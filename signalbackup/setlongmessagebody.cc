#include "signalbackup.ih"

#include "../common_be.h"

void SignalBackup::setLongMessageBody(std::string *body, SqliteDB::QueryResults *attachment_results) const
{
  for (uint ai = 0; ai < attachment_results->rows(); ++ai)
  {
    if (attachment_results->valueAsString(ai, d_part_ct) == "text/x-signal-plain") [[unlikely]]
    {
      //std::cout << "Got long message!" << std::endl;
      SqliteDB::QueryResults longmessage = attachment_results->getRow(ai);
      attachment_results->removeRow(ai);
      // get message:
      long long int rowid = longmessage.valueAsInt(0, "_id");
      long long int uniqueid = longmessage.valueAsInt(0, "unique_id");
      if (!bepaald::contains(d_attachments, std::pair{rowid, uniqueid})) [[unlikely]]
        continue;
      AttachmentFrame *a = d_attachments.at({rowid, uniqueid}).get();
      *body = std::string(reinterpret_cast<char *>(a->attachmentData()), a->attachmentSize());
      a->clearData();
      break; // always max 1?
    }
  }
}
