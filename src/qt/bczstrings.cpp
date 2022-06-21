

#include <QtGlobal>

// Automatically generated by extract_strings.py
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif
static const char UNUSED *bcz_strings[] = {
QT_TRANSLATE_NOOP("bcz-core", ""
"Cannot find the Sapling parameters in the following directory:\n"
"%s\n"
"Please run 'sapling-fetch-params' or './util/fetch-params.sh' and then "
"restart."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Cannot obtain a lock on data directory %s. %s is probably already running."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Cannot start deterministic masternode before enforcement. Remove %s to start "
"as legacy masternode"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Cannot upgrade to Sapling wallet (already running Sapling support). Version: "
"%d"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Distributed under the MIT software license, see the accompanying file "
"COPYING or <http://www.opensource.org/licenses/mit-license.php>."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Enabling Masternode support requires turning on transaction indexing.Please "
"add %s to your configuration and start with %s"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Error: Listening for incoming connections failed (listen returned error %s)"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Error: Unsupported argument %s found. Setting SOCKS version isn't possible "
"anymore, only SOCKS5 proxies are supported."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Failed to create backup, file already exists! This could happen if you "
"restarted wallet in less than 60 seconds. You can continue if you are ok "
"with this."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Invalid -wallet path '%s'. -wallet path should point to a directory where "
"wallet.dat and database/log.?????????? files can be stored, a location where "
"such a directory could be created or (for backwards compatibility) the name "
"of an existing data file in -walletdir (%s)"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Invalid amount for %s: '%s' (must be at least the minimum relay fee of %s to "
"prevent stuck transactions)"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Legacy masternode system disabled. Use %s to start as deterministic "
"masternode"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Please check that your computer's date and time are correct! If your clock "
"is wrong %s will not work properly."),
QT_TRANSLATE_NOOP("bcz-core", ""
"System error while flushing the chainstate after pruning invalid entries. "
"Possible corrupt database."),
QT_TRANSLATE_NOOP("bcz-core", ""
"The block database contains a block which appears to be from the future. "
"This may be due to your computer's date and time being set incorrectly. Only "
"rebuild the block database if you are sure that your computer's date and "
"time are correct"),
QT_TRANSLATE_NOOP("bcz-core", ""
"This file contains all of your private keys in plain text. DO NOT send this "
"file to anyone!"),
QT_TRANSLATE_NOOP("bcz-core", ""
"This is a pre-release test build - use at your own risk - do not use for "
"staking or merchant applications!"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Total length of network version string (%i) exceeds maximum length (%i). "
"Reduce the number or size of %s."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Unable to replay blocks. You will need to rebuild the database using %s."),
QT_TRANSLATE_NOOP("bcz-core", ""
"WARNING: The transaction has been signed and recorded, so the wallet will "
"try to re-send it. Use 'abandontransaction' to cancel it. (txid: %s)"),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: %s is set very high! Fees this large could be paid on a single "
"transaction."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: %s is set very high! This is the transaction fee you will pay if "
"you send a transaction."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: Please check that your computer's date and time are correct! If "
"your clock is wrong %s will not work properly."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: The network does not appear to fully agree! Some miners appear to "
"be experiencing issues."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: Wallet file corrupt, data salvaged! Original %s saved as %s in %s; "
"if your balance or transactions are incorrect you should restore from a "
"backup."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: We do not appear to fully agree with our peers! You may need to "
"upgrade, or other nodes may need to upgrade."),
QT_TRANSLATE_NOOP("bcz-core", ""
"Warning: error reading %s! All keys read correctly, but transaction data or "
"address book entries might be missing or incorrect."),
QT_TRANSLATE_NOOP("bcz-core", ""
"\"local\" vote is no longer available with DMNs. Use \"alias\" from the "
"wallet with the voting key."),
QT_TRANSLATE_NOOP("bcz-core", "%s Invalid %s address or hostname: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "%s corrupt, salvage failed"),
QT_TRANSLATE_NOOP("bcz-core", "%s is not allowed in combination with enabled wallet functionality"),
QT_TRANSLATE_NOOP("bcz-core", "%s is only allowed with a single wallet file"),
QT_TRANSLATE_NOOP("bcz-core", "(must be %d for %s-net)"),
QT_TRANSLATE_NOOP("bcz-core", "A fatal internal error occurred, see debug.log for details"),
QT_TRANSLATE_NOOP("bcz-core", "Active Masternode not initialized."),
QT_TRANSLATE_NOOP("bcz-core", "Calculating money supply..."),
QT_TRANSLATE_NOOP("bcz-core", "Can't generate a change-address key. Please call keypoolrefill first."),
QT_TRANSLATE_NOOP("bcz-core", "Cannot downgrade wallet."),
QT_TRANSLATE_NOOP("bcz-core", "Cannot resolve -%s address: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "Cannot set %s or %s together with %s"),
QT_TRANSLATE_NOOP("bcz-core", "Cannot upgrade a locked wallet."),
QT_TRANSLATE_NOOP("bcz-core", "Cannot write to data directory '%s'; check permissions."),
QT_TRANSLATE_NOOP("bcz-core", "Change index out of range"),
QT_TRANSLATE_NOOP("bcz-core", "Copyright (C) 2009-%i The Bitcoin Core Developers"),
QT_TRANSLATE_NOOP("bcz-core", "Copyright (C) 2014-%i The Dash Core Developers"),
QT_TRANSLATE_NOOP("bcz-core", "Copyright (C) 2015-%i The %s Developers"),
QT_TRANSLATE_NOOP("bcz-core", "Corrupted block database detected"),
QT_TRANSLATE_NOOP("bcz-core", "Could not find asmap file %s"),
QT_TRANSLATE_NOOP("bcz-core", "Could not open debug log file %s"),
QT_TRANSLATE_NOOP("bcz-core", "Could not parse asmap file %s"),
QT_TRANSLATE_NOOP("bcz-core", "Could not parse masternode.conf"),
QT_TRANSLATE_NOOP("bcz-core", "Deterministic masternodes are not enforced yet"),
QT_TRANSLATE_NOOP("bcz-core", "Do you want to rebuild the block database now?"),
QT_TRANSLATE_NOOP("bcz-core", "Done loading"),
QT_TRANSLATE_NOOP("bcz-core", "Elliptic curve cryptography sanity check failure. Aborting."),
QT_TRANSLATE_NOOP("bcz-core", "Error initializing block database"),
QT_TRANSLATE_NOOP("bcz-core", "Error initializing wallet database environment %s!"),
QT_TRANSLATE_NOOP("bcz-core", "Error loading %s: Wallet corrupted"),
QT_TRANSLATE_NOOP("bcz-core", "Error loading %s: Wallet requires newer version of %s"),
QT_TRANSLATE_NOOP("bcz-core", "Error loading %s\n"),
QT_TRANSLATE_NOOP("bcz-core", "Error loading block database"),
QT_TRANSLATE_NOOP("bcz-core", "Error loading wallet %s. Duplicate %s filename specified."),
QT_TRANSLATE_NOOP("bcz-core", "Error opening block database"),
QT_TRANSLATE_NOOP("bcz-core", "Error reading from database, shutting down."),
QT_TRANSLATE_NOOP("bcz-core", "Error upgrading chainstate database"),
QT_TRANSLATE_NOOP("bcz-core", "Error"),
QT_TRANSLATE_NOOP("bcz-core", "Error: %s must be at least %d MB"),
QT_TRANSLATE_NOOP("bcz-core", "Error: %s must be true if %s is set."),
QT_TRANSLATE_NOOP("bcz-core", "Error: A fatal internal error occured, see debug.log for details"),
QT_TRANSLATE_NOOP("bcz-core", "Error: A fatal internal error occurred, see debug.log for details"),
QT_TRANSLATE_NOOP("bcz-core", "Error: Disk space is low for %s"),
QT_TRANSLATE_NOOP("bcz-core", "Error: Disk space is low!"),
QT_TRANSLATE_NOOP("bcz-core", "Error: Initializing networking failed"),
QT_TRANSLATE_NOOP("bcz-core", "Error: Unsupported argument %s found, use %s."),
QT_TRANSLATE_NOOP("bcz-core", "Error: Unsupported argument %s found. Checklevel must be level 4."),
QT_TRANSLATE_NOOP("bcz-core", "Failed to accept tx in the memory pool (reason: %s)\n"),
QT_TRANSLATE_NOOP("bcz-core", "Failed to delete backup, error: %s"),
QT_TRANSLATE_NOOP("bcz-core", "Failed to listen on any port. Use %s if you want this."),
QT_TRANSLATE_NOOP("bcz-core", "Failed to parse host:port string"),
QT_TRANSLATE_NOOP("bcz-core", "Failed to rescan the wallet during initialization"),
QT_TRANSLATE_NOOP("bcz-core", "Importing..."),
QT_TRANSLATE_NOOP("bcz-core", "Incorrect or no genesis block found. Wrong datadir for network?"),
QT_TRANSLATE_NOOP("bcz-core", "Information"),
QT_TRANSLATE_NOOP("bcz-core", "Initialization sanity check failed. %s is shutting down."),
QT_TRANSLATE_NOOP("bcz-core", "Insufficient funds."),
QT_TRANSLATE_NOOP("bcz-core", "Invalid -masternodeaddr address: %s"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid -masternodeaddr port %d, isn't the same as the peer port %d"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid -masternodeaddr port %d, only %d is supported on %s-net."),
QT_TRANSLATE_NOOP("bcz-core", "Invalid activation height (%s)"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid amount for %s: '%s' (must be at least %s)"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid amount for -%s=<amount>: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid masternodeprivkey. Please see the documentation."),
QT_TRANSLATE_NOOP("bcz-core", "Invalid mnoperatorprivatekey. Please see the documentation."),
QT_TRANSLATE_NOOP("bcz-core", "Invalid netmask specified in %s: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid network upgrade (%s)"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid port %d detected in masternode.conf"),
QT_TRANSLATE_NOOP("bcz-core", "Invalid status error."),
QT_TRANSLATE_NOOP("bcz-core", "Keypool ran out, please call keypoolrefill first, or unlock the wallet."),
QT_TRANSLATE_NOOP("bcz-core", "Legacy Masternode is obsolete."),
QT_TRANSLATE_NOOP("bcz-core", "Line: %d"),
QT_TRANSLATE_NOOP("bcz-core", "Loading addresses..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading banlist..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading block index..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading masternode cache..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading masternode payment cache..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading sporks..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading wallet..."),
QT_TRANSLATE_NOOP("bcz-core", "Loading/Pruning invalid outputs..."),
QT_TRANSLATE_NOOP("bcz-core", "MNs synchronization pending..."),
QT_TRANSLATE_NOOP("bcz-core", "Need to specify a port with %s: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "Network upgrade parameters malformed, expecting %s"),
QT_TRANSLATE_NOOP("bcz-core", "Network upgrade parameters may only be overridden on regtest."),
QT_TRANSLATE_NOOP("bcz-core", "No error"),
QT_TRANSLATE_NOOP("bcz-core", "Not enough file descriptors available."),
QT_TRANSLATE_NOOP("bcz-core", "OS cryptographic RNG sanity check failure. Aborting."),
QT_TRANSLATE_NOOP("bcz-core", "Peers are being disconnected due time differences."),
QT_TRANSLATE_NOOP("bcz-core", "Preparing for resync..."),
QT_TRANSLATE_NOOP("bcz-core", "Reaccepting wallet transactions..."),
QT_TRANSLATE_NOOP("bcz-core", "Replaying blocks..."),
QT_TRANSLATE_NOOP("bcz-core", "Rescanning..."),
QT_TRANSLATE_NOOP("bcz-core", "Shutdown requested over the txs scan. Exiting."),
QT_TRANSLATE_NOOP("bcz-core", "Signing transaction failed"),
QT_TRANSLATE_NOOP("bcz-core", "Specified -walletdir \"%s\" does not exist"),
QT_TRANSLATE_NOOP("bcz-core", "Specified -walletdir \"%s\" is a relative path"),
QT_TRANSLATE_NOOP("bcz-core", "Specified -walletdir \"%s\" is not a directory"),
QT_TRANSLATE_NOOP("bcz-core", "Specified blocks directory \"%s\" does not exist."),
QT_TRANSLATE_NOOP("bcz-core", "Synchronization failed"),
QT_TRANSLATE_NOOP("bcz-core", "Synchronization finished"),
QT_TRANSLATE_NOOP("bcz-core", "Synchronizing masternode winners..."),
QT_TRANSLATE_NOOP("bcz-core", "Synchronizing masternodes..."),
QT_TRANSLATE_NOOP("bcz-core", "Synchronizing sporks..."),
QT_TRANSLATE_NOOP("bcz-core", "The threshold value cannot be less than %s"),
QT_TRANSLATE_NOOP("bcz-core", "This is experimental software."),
QT_TRANSLATE_NOOP("bcz-core", "This is not a deterministic masternode. 'local' option disabled."),
QT_TRANSLATE_NOOP("bcz-core", "This is not a masternode. 'local' option disabled."),
QT_TRANSLATE_NOOP("bcz-core", "This is not a masternode."),
QT_TRANSLATE_NOOP("bcz-core", "This product includes UPnP software written by Thomas Bernard."),
QT_TRANSLATE_NOOP("bcz-core", "Transaction amount too small"),
QT_TRANSLATE_NOOP("bcz-core", "Transaction amounts must be positive"),
QT_TRANSLATE_NOOP("bcz-core", "Transaction canceled."),
QT_TRANSLATE_NOOP("bcz-core", "Transaction must have at least one recipient"),
QT_TRANSLATE_NOOP("bcz-core", "Transaction too large for fee policy"),
QT_TRANSLATE_NOOP("bcz-core", "Transaction too large"),
QT_TRANSLATE_NOOP("bcz-core", "Unable to bind to %s on this computer (bind returned error %s)"),
QT_TRANSLATE_NOOP("bcz-core", "Unable to bind to %s on this computer. %s is probably already running."),
QT_TRANSLATE_NOOP("bcz-core", "Unable to generate initial key!"),
QT_TRANSLATE_NOOP("bcz-core", "Unable to generate keys"),
QT_TRANSLATE_NOOP("bcz-core", "Unable to sign spork message, wrong key?"),
QT_TRANSLATE_NOOP("bcz-core", "Unable to start HTTP server. See debug log for details."),
QT_TRANSLATE_NOOP("bcz-core", "Unknown network specified in %s: '%s'"),
QT_TRANSLATE_NOOP("bcz-core", "Unsupported logging category %s=%s."),
QT_TRANSLATE_NOOP("bcz-core", "Upgrading coins database if needed..."),
QT_TRANSLATE_NOOP("bcz-core", "User Agent comment (%s) contains unsafe characters."),
QT_TRANSLATE_NOOP("bcz-core", "Verifying blocks..."),
QT_TRANSLATE_NOOP("bcz-core", "Verifying wallet(s)..."),
QT_TRANSLATE_NOOP("bcz-core", "Wallet %s resides outside data directory %s"),
QT_TRANSLATE_NOOP("bcz-core", "Wallet needed to be rewritten: restart %s to complete"),
QT_TRANSLATE_NOOP("bcz-core", "Warning"),
QT_TRANSLATE_NOOP("bcz-core", "Warning: This version is obsolete, upgrade required!"),
QT_TRANSLATE_NOOP("bcz-core", "Warning: Unsupported argument %s ignored, use %s"),
QT_TRANSLATE_NOOP("bcz-core", "Warning: Unsupported argument %s ignored, use %s."),
QT_TRANSLATE_NOOP("bcz-core", "You can not start a masternode in litemode"),
QT_TRANSLATE_NOOP("bcz-core", "You need to rebuild the database using %s to change %s"),
QT_TRANSLATE_NOOP("bcz-core", "Zapping all transactions from wallet..."),
};