.headers on
.mode column
SELECT a.Path,
       a.ModuleLabel,
       a.ModuleType,
       a.Vsize AS "Vsize (MB)",
       Run,
       SubRun,
       Event
FROM ModuleInfo AS a
WHERE (SELECT COUNT(*)+1
       FROM ModuleInfo AS b
       WHERE b.Step = 'PostProcessModule'
       AND b.Path = a.Path
       AND b.ModuleLabel = a.ModuleLabel
       AND b.Vsize > a.Vsize) < 4
AND a.Step = 'PostProcessModule'
AND a.Vsize > 0
ORDER BY a.Path, a.ModuleLabel ASC, a.Vsize DESC;
