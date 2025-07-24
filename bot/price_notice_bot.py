import asyncio
import json
import logging
from datetime import datetime
from typing import Optional, Dict, Any, List
import aiohttp
import aiofiles
from dataclasses import dataclass

# Telegram Bot API
from telegram import Update, Bot
from telegram.ext import Application, MessageHandler, CommandHandler, ContextTypes, filters
from telegram.error import TelegramError
from telegram.constants import ParseMode

import pybtrader
class JournalReader(object):
    ''''''
    def __init__(self, conf: str):
        self.reader_impl = pybtrader.tool.JournalReader()
        self.reader_impl.init(conf)

    def read(self) -> dict:
        return self.reader_impl.read()

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

@dataclass
class BotConfig:
    """Bot configuration class"""
    bot_token: str
    channel_id: str  # Channel ID (e.g., @channel_name or -1001234567890)
    admin_user_ids: List[int] = None  # List of admin user IDs who can control the bot
    data_source_url: Optional[str] = None  # Data source URL
    data_file_path: Optional[str] = None   # Local data file path
    data_journal_conf: Optional[str] = None # data feeding from journal
    interval: int = 60  # Send interval in seconds
    max_message_length: int = 4096  # Telegram max message length
    auto_push: bool = True  # Whether to automatically push data

class DataReader:
    """Data reader class"""
    
    def __init__(self, config: BotConfig):
        self.config = config
        self.session: Optional[aiohttp.ClientSession] = None # For http url
        self.journal_reader: Optional[JournalReader] = None # For journal
    
    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        return self
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        if self.session:
            await self.session.close()

    async def read_from_journal(self) -> Optional[str]:
        '''Read data from journal'''
        if not self.journal_reader:
            self.journal_reader = JournalReader(self.config.data_journal_conf)

        data_dict = self.journal_reader.read()
        # Parse dict
        return None
    
    async def read_from_url(self) -> Optional[str]:
        """Read data from URL"""
        if not self.config.data_source_url or not self.session:
            return None
        
        try:
            async with self.session.get(self.config.data_source_url) as response:
                if response.status == 200:
                    data = await response.text()
                    logger.info(f"Successfully read data from URL: {len(data)} characters")
                    return data
                else:
                    logger.error(f"HTTP request failed: {response.status}")
                    return None
        except Exception as e:
            logger.error(f"Failed to read data from URL: {e}")
            return None
    
    async def read_from_file(self) -> Optional[str]:
        """Read data from file"""
        if not self.config.data_file_path:
            return None
        
        try:
            async with aiofiles.open(self.config.data_file_path, 'r', encoding='utf-8') as f:
                data = await f.read()
                logger.info(f"Successfully read data from file: {len(data)} characters")
                return data
        except Exception as e:
            logger.error(f"Failed to read data from file: {e}")
            return None
    
    def generate_sample_data(self) -> str:
        """Generate sample data"""
        current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        sample_data = {
            "timestamp": current_time,
            "message": "This is a test message",
            "data": {
                "temperature": 25.5,
                "humidity": 60,
                "status": "normal",
                "cpu_usage": "45%",
                "memory_usage": "67%"
            },
            "alerts": []
        }
        return json.dumps(sample_data, ensure_ascii=False, indent=2)
    
    async def get_data(self) -> str:
        """Main method to get data"""
        # Try journal
        if self.config.data_journal_conf:
            data = await self.read_from_journal()
            if data:
                return data

        # Try URL
        if self.config.data_source_url:
            data = await self.read_from_url()
            if data:
                return data
        
        # Try file
        if self.config.data_file_path:
            data = await self.read_from_file()
            if data:
                return data
        
        # Fallback to sample data
        return self.generate_sample_data()

class TelegramInteractiveBot:
    """Telegram interactive bot that can send and receive messages"""
    
    def __init__(self, config: BotConfig):
        self.config = config
        self.application = Application.builder().token(config.bot_token).build()
        self.running = False
        self.auto_push_running = False
        self.data_reader = None
        self.received_messages = []  # Store received messages
        
        # Setup handlers
        self._setup_handlers()
    
    def _setup_handlers(self):
        """Setup message and command handlers"""
        # Command handlers
        self.application.add_handler(CommandHandler("start", self.start_command))
        self.application.add_handler(CommandHandler("stop", self.stop_command))
        self.application.add_handler(CommandHandler("status", self.status_command))
        self.application.add_handler(CommandHandler("push", self.manual_push_command))
        self.application.add_handler(CommandHandler("autopush", self.toggle_autopush_command))
        self.application.add_handler(CommandHandler("messages", self.show_messages_command))
        self.application.add_handler(CommandHandler("help", self.help_command))
        
        # Message handler for channel messages
        self.application.add_handler(
            MessageHandler(filters.ALL & ~filters.COMMAND, self.handle_message)
        )
    
    def _is_admin(self, user_id: int) -> bool:
        """Check if user is admin"""
        if not self.config.admin_user_ids:
            return True  # If no admin list, allow all users
        return user_id in self.config.admin_user_ids
    
    async def start_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /start command"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        message = """🤖 <b>Telegram Interactive Bot Started</b>

<b>Available Commands:</b>
/start - Start the bot
/stop - Stop auto push
/status - Show bot status
/push - Manual data push
/autopush - Toggle auto push on/off
/messages - Show recent received messages
/help - Show this help message

Bot is now ready to receive and send messages! 🚀"""
        
        await update.message.reply_text(message, parse_mode=ParseMode.HTML)
        logger.info(f"Bot started by user {user_id}")
    
    async def stop_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /stop command"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        self.auto_push_running = False
        await update.message.reply_text("⏹️ Auto push stopped!")
        logger.info(f"Auto push stopped by user {user_id}")
    
    async def status_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /status command"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        status_message = f"""📊 <b>Bot Status Report</b>
⏰ <b>Current Time:</b> <code>{datetime.now().strftime("%Y-%m-%d %H:%M:%S")}</code>
🔄 <b>Auto Push:</b> <code>{'ON' if self.auto_push_running else 'OFF'}</code>
📡 <b>Data Source:</b> <code>{self._get_data_source()}</code>
🔔 <b>Push Interval:</b> <code>{self.config.interval} seconds</code>
📬 <b>Received Messages:</b> <code>{len(self.received_messages)}</code>
🎯 <b>Target Channel:</b> <code>{self.config.channel_id}</code>"""
        
        await update.message.reply_text(status_message, parse_mode=ParseMode.HTML)
    
    async def manual_push_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /push command for manual data push"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        await update.message.reply_text("📤 Pushing data manually...")
        
        try:
            if not self.data_reader:
                async with DataReader(self.config) as reader:
                    data = await reader.get_data()
            else:
                data = await self.data_reader.get_data()
            
            message = self._format_data_message(data)
            success = await self.send_to_channel(message)
            
            if success:
                await update.message.reply_text("✅ Data pushed successfully!")
            else:
                await update.message.reply_text("❌ Failed to push data. Check logs for details.")
                
        except Exception as e:
            logger.error(f"Manual push failed: {e}")
            await update.message.reply_text(f"❌ Push failed: {str(e)}")
    
    async def toggle_autopush_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /autopush command to toggle auto push"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        if self.auto_push_running:
            self.auto_push_running = False
            await update.message.reply_text("⏸️ Auto push disabled!")
        else:
            self.auto_push_running = True
            await update.message.reply_text("▶️ Auto push enabled!")
            # Start auto push task if not already running
            if not hasattr(self, '_auto_push_task') or self._auto_push_task.done():
                self._auto_push_task = asyncio.create_task(self._auto_push_loop())
    
    async def show_messages_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /messages command to show recent received messages"""
        user_id = update.effective_user.id
        if not self._is_admin(user_id):
            await update.message.reply_text("❌ You don't have permission to use this bot.")
            return
        
        if not self.received_messages:
            await update.message.reply_text("📭 No messages received yet.")
            return
        
        # Show last 5 messages
        recent_messages = self.received_messages[-5:]
        message_text = "📬 <b>Recent Received Messages:</b>\n\n"
        
        for i, msg_info in enumerate(recent_messages, 1):
            message_text += f"<b>{i}.</b> <code>{msg_info['timestamp']}</code>\n"
            message_text += f"👤 <b>From:</b> {msg_info['from']}\n"
            message_text += f"💬 <b>Message:</b> <code>{msg_info['text'][:100]}{'...' if len(msg_info['text']) > 100 else ''}</code>\n\n"
        
        await update.message.reply_text(message_text, parse_mode=ParseMode.HTML)
    
    async def help_command(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle /help command"""
        help_text = """🤖 <b>Telegram Interactive Bot Help</b>

<b>Commands:</b>
/start - Initialize the bot
/stop - Stop automatic data pushing
/status - Show current bot status
/push - Manually push data to channel
/autopush - Toggle automatic pushing on/off
/messages - Show recent received messages
/help - Show this help message

<b>Features:</b>
• Automatically push data to configured channel
• Receive and log messages from channels/users
• Admin control with permission system
• Multiple data sources (URL, file, sample data)
• Message formatting and splitting for long content

<b>Bot Status:</b>
Auto Push: {'ON' if self.auto_push_running else 'OFF'}
Data Source: {self._get_data_source()}"""
        
        await update.message.reply_text(help_text, parse_mode=ParseMode.HTML)
    
    async def handle_message(self, update: Update, context: ContextTypes.DEFAULT_TYPE):
        """Handle received messages"""
        message = update.message
        if not message:
            return
        
        # Store message info
        message_info = {
            "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "from": f"{message.from_user.first_name} (@{message.from_user.username})" if message.from_user.username else message.from_user.first_name,
            "chat_type": message.chat.type,
            "chat_id": message.chat_id,
            "text": message.text or "[Media/File]",
            "message_id": message.message_id
        }
        
        self.received_messages.append(message_info)
        
        # Keep only last 50 messages
        if len(self.received_messages) > 50:
            self.received_messages = self.received_messages[-50:]
        
        logger.info(f"Received message from {message_info['from']}: {message_info['text'][:50]}...")
        
        # Auto-reply for certain keywords (optional)
        if message.text and any(keyword in message.text.lower() for keyword in ['help', 'status', 'info']):
            reply_text = """🤖 <b>Auto Reply</b>
I received your message! Use /help to see available commands.
Admins can control the bot using commands."""
            
            try:
                await message.reply_text(reply_text, parse_mode=ParseMode.HTML)
            except TelegramError as e:
                logger.error(f"Failed to send auto-reply: {e}")
    
    def _get_data_source(self) -> str:
        """Get data source description"""
        if self.config.data_source_url:
            return f"API Endpoint ({self.config.data_source_url})"
        elif self.config.data_file_path:
            return f"Local File ({self.config.data_file_path})"
        else:
            return "Sample Data"
    
    def _format_data_message(self, data: str) -> str:
        """Format data message"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        try:
            json_data = json.loads(data)
            formatted_data = json.dumps(json_data, ensure_ascii=False, indent=2)
            
            message = f"""📊 <b>Data Update Report</b>
⏰ <b>Time:</b> <code>{timestamp}</code>

📋 <b>Data Content:</b>
<pre><code class="language-json">{formatted_data}</code></pre>

<i>Data Source: {self._get_data_source()}</i>"""
            
        except json.JSONDecodeError:
            message = f"""📊 <b>Data Update Report</b>
⏰ <b>Time:</b> <code>{timestamp}</code>

📋 <b>Data Content:</b>
<pre>{data}</pre>

<i>Data Source: {self._get_data_source()}</i>"""
        
        return message
    
    async def send_to_channel(self, text: str, parse_mode: ParseMode = ParseMode.HTML) -> bool:
        """Send message to configured channel"""
        try:
            if len(text) > self.config.max_message_length:
                messages = self._split_message(text)
                success = True
                for i, msg in enumerate(messages):
                    if i > 0:
                        await asyncio.sleep(0.5)
                    
                    try:
                        await self.application.bot.send_message(
                            chat_id=self.config.channel_id,
                            text=msg,
                            parse_mode=parse_mode
                        )
                        logger.info(f"Message part {i+1}/{len(messages)} sent to channel")
                    except TelegramError as e:
                        logger.error(f"Failed to send message part: {e}")
                        success = False
                
                return success
            else:
                await self.application.bot.send_message(
                    chat_id=self.config.channel_id,
                    text=text,
                    parse_mode=parse_mode
                )
                logger.info("Message sent to channel successfully")
                return True
                
        except TelegramError as e:
            logger.error(f"Failed to send message to channel: {e}")
            return False
    
    def _split_message(self, text: str) -> list:
        """Split long message"""
        messages = []
        while len(text) > self.config.max_message_length:
            split_pos = text.rfind('\n', 0, self.config.max_message_length - 100)
            if split_pos == -1:
                split_pos = self.config.max_message_length - 100
            
            messages.append(text[:split_pos])
            text = text[split_pos:].lstrip('\n')
        
        if text:
            messages.append(text)
        
        return messages
    
    async def _auto_push_loop(self):
        """Auto push loop"""
        logger.info("Auto push loop started")
        
        async with DataReader(self.config) as reader:
            self.data_reader = reader
            
            while self.auto_push_running:
                try:
                    data = await reader.get_data()
                    message = self._format_data_message(data)
                    success = await self.send_to_channel(message)
                    
                    if success:
                        logger.info("Auto push successful")
                    else:
                        logger.error("Auto push failed")
                    
                    # Wait for next push
                    for i in range(self.config.interval):
                        if not self.auto_push_running:
                            break
                        await asyncio.sleep(1)
                        
                except Exception as e:
                    logger.error(f"Auto push exception: {e}")
                    await asyncio.sleep(5)
        
        logger.info("Auto push loop stopped")
    
    async def start_bot(self):
        """Start the bot"""
        logger.info("Starting Telegram Interactive Bot...")
        
        try:
            # Test connection
            bot_info = await self.application.bot.get_me()
            logger.info(f"Bot connected: @{bot_info.username}")
            
            # Send startup message to channel
            startup_message = f"""🚀 <b>Interactive Bot Started</b>
⏰ <b>Start Time:</b> <code>{datetime.now().strftime("%Y-%m-%d %H:%M:%S")}</code>
🤖 <b>Bot:</b> @{bot_info.username}
📡 <b>Data Source:</b> {self._get_data_source()}

Bot is ready to receive commands and messages! 
Use /help for available commands."""
            
            await self.send_to_channel(startup_message)
            
            # Start auto push if enabled
            if self.config.auto_push:
                self.auto_push_running = True
                self._auto_push_task = asyncio.create_task(self._auto_push_loop())
            
            # Start polling
            self.running = True
            await self.application.run_polling(drop_pending_updates=True)
            
        except Exception as e:
            logger.error(f"Failed to start bot: {e}")
        finally:
            self.running = False
            if hasattr(self, '_auto_push_task'):
                self._auto_push_task.cancel()
    
    def stop_bot(self):
        """Stop the bot"""
        self.running = False
        self.auto_push_running = False
        if self.application.updater:
            self.application.stop()

# Configuration and main execution
class BotManager:
    """Bot manager"""
    
    def __init__(self):
        self.bot = None
    
    def create_config(self) -> BotConfig:
        """Create bot configuration"""
        return BotConfig(
            # Required configuration
            bot_token="YOUR_BOT_TOKEN_HERE",
            channel_id="@your_channel_name",
            
            # Optional: Admin user IDs (get from @userinfobot)
            admin_user_ids=[123456789, 987654321],  # Replace with actual user IDs
            
            # Data source configuration
            data_source_url="https://jsonplaceholder.typicode.com/posts/1",
            data_file_path="data.json",
            
            # Bot behavior
            interval=60,  # Auto push interval
            auto_push=True,  # Start with auto push enabled
        )
    
    async def run(self):
        """Run the bot"""
        config = self.create_config()
        
        # Validate configuration
        if config.bot_token == "YOUR_BOT_TOKEN_HERE":
            logger.error("Please configure Bot Token!")
            return
        
        if config.channel_id == "@your_channel_name":
            logger.error("Please configure Channel ID!")
            return
        
        self.bot = TelegramInteractiveBot(config)
        
        try:
            await self.bot.start_bot()
        except KeyboardInterrupt:
            logger.info("Shutting down bot...")
            if self.bot:
                self.bot.stop_bot()

async def main():
    """Main function"""
    manager = BotManager()
    await manager.run()

if __name__ == "__main__":
    print("🤖 Telegram Interactive Bot (Send & Receive)")
    print("=" * 60)
    print("Features:")
    print("✅ Send data to channel automatically")
    print("✅ Receive and log messages from users/channels")
    print("✅ Admin commands for bot control")
    print("✅ Manual and automatic data pushing")
    print("✅ Message history tracking")
    print("=" * 60)
    print("Setup: Configure bot_token, channel_id, and admin_user_ids")
    print("Commands: /start, /stop, /status, /push, /autopush, /messages, /help")
    print("=" * 60)
    
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n👋 Bot stopped")