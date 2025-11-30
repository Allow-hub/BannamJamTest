using System.Text.RegularExpressions;
using Spectre.Console;

if (args.Length == 0)
{
    AnsiConsole.MarkupLine("[yellow]usage: doc <ClassName>[/]");
    return;
}

string target = args[0];
string docsPath = Directory.GetCurrentDirectory();

var mdFiles = Directory.GetFiles(docsPath, "*.md", SearchOption.AllDirectories);

if (mdFiles.Length == 0)
{
    AnsiConsole.MarkupLine("[red]No markdown files found in current directory.[/]");
    AnsiConsole.MarkupLine("Please run this command from the 'doc' folder.");
    return;
}

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