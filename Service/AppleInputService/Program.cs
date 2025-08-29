using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

await Host.CreateDefaultBuilder(args)
    .UseWindowsService()
    .ConfigureLogging(lb => lb.AddSimpleConsole())
    .Build()
    .RunAsync();