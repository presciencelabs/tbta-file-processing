# Word Doc processing for TBTA
Written by Nathan Davis, modified by Craig Palmer

# Features and TODO
There is a desire to "clean up" the strings in cells before insertion into the TbtaTable.  For example, eliminate multiple spaces and remove smart quotes.  This would be easy enough to do for the table text itself - but it could mess up the processing of segments if not done carefully, so need to consider that.

Currently, I am only processing table contents.  But looking at the sample files, do I also need to process table "labels"?

Some errors are still only getting reported by printing a message to stdout.  I need to convert these into proper exceptions.

Need to test filenames containing non-ascii characters.

Check other "TODO" items.

After docx processing is fairly well stabilized, want to add .odt processing to expand useability to more users.

Want to add docx (and .odt) export - but need to clarify what exactly is expected here.

# Getting Nathan's changes from Bitbucket (Obsolete)
(Recorded here for historical purposes)
To get Nathan's files, right click on the folder name, start TortoiseHg workbench, and click on the "pull incoming changes" button.  My password is (removed).
Build ReleaseWithCString and call it TbtaFileProcessing.lib, and also build the Debug version, and call it TbtaFileProcessingDebug.lib.
basically there are two steps when updating my local code, pull and update

When I make changes, right click on the directory name and click Hg Commit and add a comment, which makes a local revision, then click Commit
Then right click on the directory again and start Hg Workbench, and click the Push button.
basically there are two steps when updating the repository, commit and push

get 2013 community version, but this will take several hours