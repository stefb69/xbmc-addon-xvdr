#pragma once
/*
 *      xbmc-addon-xvdr - XVDR addon for XBMC
 *
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2012 Alexander Pipelka
 *
 *      https://github.com/pipelka/xbmc-addon-xvdr
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "xvdr/session.h"
#include "xvdr/thread.h"

#include <string>
#include <map>
#include <vector>

#include "xvdr/dataset.h"

class MsgPacket;

namespace XVDR {

class ClientInterface;

class Connection : public Session, public Thread
{
public:

  Connection(ClientInterface* client);
  virtual ~Connection();

  bool        Open(const std::string& hostname, const std::string& name = "");
  void        Abort();
  bool        Aborting();

  void SetTimeout(int ms);
  void SetCompressionLevel(int level);
  void SetAudioType(int type);

  int                GetProtocol()   { return m_protocol; }
  const std::string& GetServerName() { return m_server; }
  const std::string& GetVersion()    { return m_version; }

  bool        EnableStatusInterface(bool onOff);
  bool        SetUpdateChannels(uint8_t method);
  bool        ChannelFilter(bool fta, bool nativelangonly, std::vector<int>& caids);

  bool        SupportChannelScan();
  bool        GetDriveSpace(long long *total, long long *used);

  int         GetChannelsCount();
  bool        GetChannelsList(bool radio = false);
  bool        GetEPGForChannel(uint32_t channeluid, time_t start, time_t end);

  int         GetChannelGroupCount(bool automatic);
  bool        GetChannelGroupList(bool bRadio);
  bool        GetChannelGroupMembers(const std::string& groupname, bool radio);

  bool        GetTimersList();
  int         GetTimersCount();
  bool        AddTimer(const Timer& timerinfo);
  bool        GetTimerInfo(unsigned int timernumber, Timer& tag);
  int         DeleteTimer(uint32_t timerindex, bool force = false);
  bool        UpdateTimer(const Timer& timerinfo);

  int         GetRecordingsCount();
  bool        GetRecordingsList();
  bool        RenameRecording(const std::string& recid, const std::string& newname);
  int         DeleteRecording(const std::string& recid);
  bool        SetRecordingPlayCount(const std::string& recid, int count);
  bool        SetRecordingLastPosition(const std::string& recid, int64_t pos);
  int64_t     GetRecordingLastPosition(const std::string& recid);

  MsgPacket*  ReadResult(MsgPacket* vrp);

  // Recordings

  bool OpenRecording(const std::string& recid);
  bool CloseRecording();

  int ReadRecording(unsigned char* buf, uint32_t buf_size);
  long long SeekRecording(long long pos, uint32_t whence);
  long long RecordingPosition(void);
  long long RecordingLength(void);
  bool LoadRecordingEdl(const std::string& recid, RecordingEdl& edl);

  // Channelscanner

  bool GetChannelScannerSetup(ChannelScannerSetup& setup, ChannelScannerList& satellites, ChannelScannerList& countries);
  bool GetChannelScannerSetup(ChannelScannerSetup& setup);
  bool SetChannelScannerSetup(const ChannelScannerSetup& setup);
  bool GetChannelScannerStatus(ChannelScannerStatus& status);
  bool StartChannelScanner();
  bool StopChannelScanner();

protected:

  virtual void Action(void);
  virtual bool OnResponsePacket(MsgPacket *pkt);
  virtual bool TryReconnect();

  void SignalConnectionLost();
  void OnDisconnect();
  void OnReconnect();

  bool m_statusinterface;
  ClientInterface* m_client;

private:

  bool        Login();

  struct SMessage
  {
    CondWait* event;
    MsgPacket* pkt;
  };
  typedef std::map<int, SMessage> SMessages;
  SMessages m_queue;

  Mutex m_mutex;
  Mutex m_cmdlock;

  bool m_aborting;
  uint32_t m_timercount;
  uint8_t m_updatechannels;
  bool m_ftachannels;
  bool m_nativelang;
  std::vector<int> m_caids;

  std::string m_recid;
  uint64_t m_currentPlayingRecordBytes;
  uint64_t m_currentPlayingRecordPosition;

  std::string m_server;
  std::string m_version;
  std::string m_name;

  int m_compressionlevel;
  int m_audiotype;
  int m_protocol;
};

} // namespace XVDR
