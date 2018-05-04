#!/usr/bin/ruby 

require 'rubygems'
require 'sqlite3'

chmode = true

ARGF.each do |l|

  msg = l.split(%r{\s*\|\s*},10) # "10" limit allows user-supplied messages to contain "|" delimiter
  timestamp = msg[0].to_s.strip
  hostname  = msg[1].to_s.strip
  hostaddr  = msg[2].to_s.strip
  severity  = msg[3].to_s.strip
  category  = msg[4].to_s.strip
  optappname= msg[5].to_s.strip
  optprocid = msg[6].to_s.strip
  optrunev  = msg[7].to_s.strip
  softmod   = msg[8].to_s.strip
  message   = msg[9].to_s.strip
  
  # Begin sqlite3 
  begin

    db = SQLite3::Database.open "/var/log/local0/messageFacilityLog.db"
    if chmode
      # allow others to read database
      File.chmod( 0644, "/var/log/local0/messageFacilityLog.db" )
      chmode = false
    end
    
    # Create header table 
    db.execute( "CREATE TABLE IF NOT EXISTS MessageHeaders(
    Timestamp TEXT, 
    Hostname TEXT, 
    Hostaddress TEXT,
    Severity TEXT,
    Category TEXT,
    AppName TEXT,
    ProcessId TEXT,
    RunEventNo TEXT,
    ModuleName TEXT
    )" )
    
    db.execute( "INSERT INTO MessageHeaders 
    (Timestamp, Hostname, Hostaddress, Severity, Category, AppName, ProcessId, RunEventNo, ModuleName )
     VALUES( ?,?,?,?,?,?,?,?,? )",
                [ timestamp, hostname, hostaddr, severity, category, optappname, optprocid, optrunev, softmod ] )
    
    hid = db.last_insert_row_id

    # Create general message table
    db.execute( "CREATE TABLE IF NOT EXISTS UserMessages(
      HeaderId INTEGER,
      Message TEXT
      )" )
      
    db.execute( "INSERT INTO UserMessages(HeaderId, Message) VALUES( ?,? )",
                [ hid, message.gsub(%r{\#012},"\n") ] )
    
  rescue SQLite3::Exception => e
    errOutput = File.open( "/var/log/local0/err.log","a+")
    errOutput << "Exception occurred\n"
    errOutput.puts e
    errOutput.close
    File.chmod( 0644, "/var/log/local0/err.log" )
    
  ensure

    db.close if db

  end

end
