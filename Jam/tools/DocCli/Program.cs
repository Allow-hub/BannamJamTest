using System.Text.RegularExpressions;
using Spectre.Console;

// コマンドライン引数のチェック
if (args.Length == 0)
{
    AnsiConsole.MarkupLine("[yellow]usage:[/]");
    AnsiConsole.MarkupLine("  [cyan]doc <ClassName>[/]     - Show documentation for a class");
    AnsiConsole.MarkupLine("  [cyan]doc --stats[/]         - Show author contribution statistics");
    return;
}

string command = args[0];
string docsPath = Directory.GetCurrentDirectory();
var mdFiles = Directory.GetFiles(docsPath, "*.md", SearchOption.AllDirectories);

if (mdFiles.Length == 0)
{
    AnsiConsole.MarkupLine("[red]No markdown files found in current directory.[/]");
    AnsiConsole.MarkupLine("Please run this command from the 'doc' folder.");
    return;
}

// 統計表示モード
if (command == "--stats" || command == "-s")
{
    var authorStats = new Dictionary<string, int>();
    var patternCreator = new Regex(@"^\s*-\s*\*\*制作者\*\*:\s*(.+)$");

    AnsiConsole.MarkupLine($"[dim]Scanning {mdFiles.Length} markdown file(s)...[/]");

    foreach (var file in mdFiles)
    {
        AnsiConsole.MarkupLine($"[dim]Reading: {Path.GetFileName(file)}[/]");
        var lines = File.ReadAllLines(file);
        int foundInFile = 0;
        foreach (var line in lines)
        {
            var match = patternCreator.Match(line);
            if (match.Success)
            {
                foundInFile++;
                var creators = match.Groups[1].Value.Split(',')
                    .Select(c => c.Trim())
                    .Where(c => !string.IsNullOrEmpty(c));

                foreach (var creator in creators)
                {
                    if (authorStats.ContainsKey(creator))
                        authorStats[creator]++;
                    else
                        authorStats[creator] = 1;
                }
            }
        }
        AnsiConsole.MarkupLine($"[dim]  Found {foundInFile} author entries[/]");
    }
    AnsiConsole.WriteLine();

    if (authorStats.Count == 0)
    {
        AnsiConsole.MarkupLine("[yellow]No author information found in documentation.[/]");
        return;
    }

    int totalItems = authorStats.Values.Sum();

    // 結果を表示
    var table = new Table();
    table.Border(TableBorder.Rounded);
    table.AddColumn(new TableColumn("[bold cyan]Author[/]").Centered());
    table.AddColumn(new TableColumn("[bold green]Count[/]").Centered());
    table.AddColumn(new TableColumn("[bold yellow]Percentage[/]").Centered());
    table.AddColumn(new TableColumn("[bold magenta]Bar[/]"));

    foreach (var author in authorStats.OrderByDescending(a => a.Value))
    {
        double percentage = (double)author.Value / totalItems * 100;
        int barLength = (int)(percentage / 2); // 50% = 25文字
        string bar = new string('█', barLength);

        table.AddRow(
            $"[cyan]{author.Key}[/]",
            $"[green]{author.Value}[/]",
            $"[yellow]{percentage:F1}%[/]",
            $"[magenta]{bar}[/]"
        );
    }

    AnsiConsole.Write(new Rule("[bold blue]Author Contribution Statistics[/]").LeftJustified());
    AnsiConsole.WriteLine();
    AnsiConsole.Write(table);
    AnsiConsole.WriteLine();
    AnsiConsole.MarkupLine($"[dim]Total items: {totalItems}[/]");
    return;
}

// 既存のクラス検索モード
string target = command;
var patternStart = new Regex(@"^###\s*.*" + Regex.Escape(target) + @".*$");
var patternNext = new Regex(@"^###\s+");
bool found = false;

foreach (var file in mdFiles)
{
    var lines = File.ReadAllLines(file);
    bool collecting = false;
    var output = new List<string>();

    for (int i = 0; i < lines.Length; i++)
    {
        var line = lines[i];
        if (!collecting && patternStart.IsMatch(line))
        {
            collecting = true;
            output.Add(line);
            continue;
        }
        if (collecting && patternNext.IsMatch(line))
        {
            break;
        }
        if (collecting)
            output.Add(line);
    }

    if (output.Count > 0)
    {
        AnsiConsole.MarkupLine($"[grey]<!-- {Path.GetFileName(file)} -->[/]");
        AnsiConsole.WriteLine();
        bool inCodeBlock = false;

        foreach (var line in output)
        {
            if (line.StartsWith("```"))
            {
                inCodeBlock = !inCodeBlock;
                AnsiConsole.MarkupLine($"[dim]{line}[/]");
            }
            else if (inCodeBlock)
            {
                // コードブロック内
                AnsiConsole.MarkupLine($"[yellow]{line}[/]");
            }
            else if (line.StartsWith("###"))
            {
                // 見出しを太字+明るい青
                var title = line.Replace("###", "").Trim();
                AnsiConsole.Write(new Rule($"[bold blue]{title}[/]").LeftJustified());
            }
            else if (line.Trim().StartsWith("-") || line.Trim().StartsWith("*"))
            {
                // リスト項目を明るい緑
                AnsiConsole.MarkupLine($"[lightgreen]{line}[/]");
            }
            else if (line.Contains("**"))
            {
                // **太字** を処理
                var formatted = Regex.Replace(line, @"\*\*(.+?)\*\*", "[bold white]$1[/]");
                AnsiConsole.MarkupLine(formatted);
            }
            else
            {
                // 通常のテキスト
                AnsiConsole.WriteLine(line);
            }
        }
        found = true;
        break;
    }
}

if (!found)
{
    AnsiConsole.MarkupLine($"[red]Class not found: {target}[/]");
}