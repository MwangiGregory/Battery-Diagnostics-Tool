from rich.console import Console
from rich.live import Live
from rich.status import Status
from rich.emoji import Emoji

console = Console()
live = Live(console=console)
stat = Status("", console=console)
emoji = Emoji('smiley')


STATUS_WARNING = "bold red"
STATUS_SUCCESS = "bold green"
TABLE_HEADER = "bold blue"
