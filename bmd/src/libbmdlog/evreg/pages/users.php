<tr>
	<th colspan="8" align="center"><b><?= $users; ?></b></th>
</tr>
<tr>
	<td colspan="8">
		<table class="hi" width="100%" align="center" cellpadding="1" cellspacing="1">
			<? $logSet = $dbHandle->select("log", "count(id) as au,log_owner, min(id) as id", "true", "log_owner", "min(id)", $offset, $limit);
			?>
			<? foreach( $logSet as $log ): ?>
				<tr>
					<? if ( $log['au'] > 1 ): ?>
						<td style="width:30px;" align="center">
							<input type="image" src="pictures/edit_add.png" title="<?= $userTitle; ?>" onclick="changeAppState(2, '<?= $log['log_owner'] ?>');">
						</td>
					<? else: ?>
						<td style="width:30px;">&nbsp;</td>
					<? endif ?>
					<td><?= htmlspecialchars($log['log_owner']) ?></td>
					<td style="width:20px"><img src="pictures/edit_user.png" /></td>
				</tr>
			<? endforeach ?>
			<? $dbHandle->__destroy(); ?>

